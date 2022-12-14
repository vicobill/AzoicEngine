// Copyright 2004-2021 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Description:  implements a point to which clients can be connected
   -------------------------------------------------------------------------
   History:
   - 26/07/2004   : Created by Craig Tiller
*************************************************************************/
#ifndef __NETNUB_H__
#define __NETNUB_H__

#pragma once

#include <CryNetwork/INetwork.h>
#include "Network.h"
#include "Socket/IDatagramSocket.h"
#include "NetChannelSecurity.h"
#include "INetworkMember.h"
#include "INubMember.h"
#include "NetTimer.h"
#include <vector>
#include "Config.h"
#include <CryLobby/CommonICryLobby.h>
#include <CryLobby/CommonICryMatchMaking.h>

class CNetChannel;
class CNetwork;
class CCryLobby;
struct INatNeg;
struct IServerReport;

enum EChannelLookupFlags
{
	eCLF_HandleBrokenWindowsICMP = 1,
};

class CNetNub : public INetNubPrivate, public INetworkMember, IDatagramListener
{
public:
	class CNubConnectingLock;

	CNetNub(
	  const TNetAddress& addr,
	  IGameNub* pGameNub,
	  IGameSecurity* pNetSecurity,
	  IGameQuery* pGameQuery);
	~CNetNub();

	// INetNub
	virtual void                        DeleteNub();
	virtual bool                        ConnectTo(const char* address, const char* connectionString);
	virtual const INetNub::SStatistics& GetStatistics();
	virtual bool                        IsConnecting();
	virtual void                        DisconnectPlayer(EDisconnectionCause cause, EntityId plr_id, const char* reason);
	virtual int                         GetNumChannels() { return int(m_channels.size()); }
	// ~INetNub

	virtual bool HasPendingConnections();

	// INetworkMember
	//virtual void Update( CTimeValue blocking );
	virtual bool         IsDead() { return m_bDead && 0 == m_keepAliveLocks && m_channels.empty(); }
	virtual bool         IsSuicidal();
	virtual void         GetMemoryStatistics(ICrySizer* pSizer);
	virtual void         NetDump(ENetDumpType type);
	void                 NetDump(ENetDumpType type, INetDumpLogger& logger);
	virtual void         PerformRegularCleanup();
	virtual void         SyncWithGame(ENetworkGameSync type);
	virtual void         GetBandwidthStatistics(SBandwidthStats* const pStats);
	virtual CNetChannel* FindFirstClientChannel();
	virtual CNetChannel* FindFirstRemoteChannel();
	//#if LOCK_NETWORK_FREQUENCY
	virtual void         TickChannels(CTimeValue& now, bool force);
	//#endif
	virtual bool         IsEstablishingContext() const;
	// ~INetworkMember

	// IDatagramListener
	virtual void OnPacket(const TNetAddress& addr, const uint8* pData, uint32 nLength);
	virtual void OnError(const TNetAddress& addr, ESocketError error);
	// ~IDatagramListener

	// INetNubPrivate
	virtual void         DisconnectChannel(EDisconnectionCause cause, CrySessionHandle session, const char* reason);
	virtual INetChannel* GetChannelFromSessionHandle(CrySessionHandle session);
	// ~INetNubPrivate

	// CryNetwork-only code
	bool                 Init(CNetwork* pNetwork);
	bool                 SendTo(const uint8* pData, size_t nSize, const TNetAddress& to);
	CNetwork*            GetNetwork()  { return m_pNetwork; }
	IGameSecurity*       GetSecurity() { return m_pSecurity; }
	void                 DisconnectChannel(EDisconnectionCause cause, CNetChannel& channel, const char* reason);
	void                 DisconnectChannel(EDisconnectionCause cause, const TNetAddress& from, CNetChannel& channel, const char* reason);
	
	void                 RegisterBackoffAddress(const TNetAddress& addr)
	{
		m_pSocketMain->RegisterBackoffAddress(addr);
	}
	void UnregisterBackoffAddress(const TNetAddress& addr)
	{
		m_pSocketMain->UnregisterBackoffAddress(addr);
	}

#if NEW_BANDWIDTH_MANAGEMENT
	void LogChannelPerformanceMetrics(uint32 id) const;
	void SetChannelPerformanceMetrics(uint32 id, INetChannel::SPerformanceMetrics& metrics);
#endif // NEW_BANDWIDTH_MANAGEMENT

	void GetLocalIPs(TNetAddressVec& vIPs)
	{
		m_pSocketMain->GetSocketAddresses(vIPs);
	}

	int GetSysSocket()
	{
		return (int)m_pSocketMain->GetSysSocket();
	}

	void      DoConnectTo(const TNetAddressVec& ips, CrySessionHandle session, string connectionString, CNubConnectingLock conlk);

	void      GN_Lazy_ContinueConnectTo(CNameRequestPtr pReq, CrySessionHandle session, string connectionString, CNubConnectingLock conlk);
	void      GN_Loop_ContinueConnectTo(CNameRequestPtr pReq, CrySessionHandle session, string connectionString, CNubConnectingLock conlk);

	IGameNub* GetGameNub() { return m_pGameNub; }
#if NEW_BANDWIDTH_MANAGEMENT
	uint32    GetTotalBandwidthShares(void) const;
#endif // NEW_BANDWIDTH_MANAGEMENT

private:
	bool         ProcessTransportPacketFrom(const TNetAddress& from, const uint8* pData, uint32 nLength);

	void         ProcessSetup(const TNetAddress& from, const uint8* pData, uint32 nLength);

	void         ProcessLanQuery(const TNetAddress& from);
	bool         ProcessPingQuery(const TNetAddress& from, const uint8* pData, uint32 nLength);
	void         ProcessDisconnect(const TNetAddress& from, const uint8* pData, uint32 nLength);
	void         ProcessDisconnectAck(const TNetAddress& from, const uint8* pData, uint32 nLength);
	void         ProcessSimplePacket(const TNetAddress& from, const uint8* pData, uint32 nLength);

	void         ProcessKeyExchange0(const TNetAddress& from, const uint8* pData, uint32 nLength);
	void         ProcessKeyExchange1(const TNetAddress& from, const uint8* pData, uint32 nLength);

	CNetChannel* GetChannelForIP(const TNetAddress& ip, uint32 flags);
	TNetAddress  GetIPForChannel(const INetChannel*);
	static void TimerCallback(NetTimerId, void*, CTimeValue);

	void CreateChannel(const TNetAddress& ip, ChannelSecurity::SInitState&& securityInit, const string& connectionString, uint32 remoteSocketCaps, uint32 proifle, CrySessionHandle session, CNubConnectingLock conlk); // first half, network thread

	void GC_CreateChannel(CNetChannel* pNetChannel, string connectionString, CNubConnectingLock conlk);   // game thread

	typedef VectorMap<TNetAddress, INubMemberPtr> TChannelMap;

	// someone that we're disconnecting
	struct SDisconnect
	{
		CTimeValue          when;
		CTimeValue          lastNotify;
		EDisconnectionCause cause;
		char                info[256];
		size_t              infoLength;
		char                backlog[1024]; // NOTE: the disconnect message will be sent within one UDP packet, our network system disallows IP fragmentation, so packets larger than the local interface MTU get discoarded
		size_t              backlogLen;
		ChannelSecurity::CHMac hmac;
	};

	enum EKeyExchangeState
	{
		eKES_NotStarted,      // a NULL state
		eKES_SetupInitiated,  // result of sending eH_ConnectionSetup, waiting for eH_ConnectionSetup_KeyExchange_0
		eKES_SentKeyExchange, // result of sending eH_ConnectionSetup_KeyExchange_0, waiting for eH_ConnectionSetup_KeyExchange_1
		eKES_KeyEstablished   // result of received eH_ConnectionSetup_KeyExchange_0, and sent eH_ConnectionSetup_KeyExchange_1 (eKES_SetupInitiated)
		                      // or received eH_ConnectionSetup_KeyExchange_1 (eKES_SentKeyExchange)
	};

	struct SKeyExchangeStuff
	{
		SKeyExchangeStuff() : kes(eKES_NotStarted), kesStart(0.0f) {}
		EKeyExchangeState                 kes;
#if ENCRYPTION_GENERATE_KEYS
		CExponentialKeyExchange           exchange;
		CExponentialKeyExchange::KEY_TYPE B, g, p, A;
#endif

		CTimeValue kesStart; // key exchange state start time, if staying in the state too long, triggers a state timeout
	};

public:
	class CNubConnectingLock
	{
	public:
		CNubConnectingLock() : m_pNub(0) {}
		CNubConnectingLock(CNetNub* pNub) : m_pNub(pNub)
		{
			if (m_pNub)
				m_pNub->LockConnecting();
		}
		CNubConnectingLock(const CNubConnectingLock& lk) : m_pNub(lk.m_pNub)
		{
			if (m_pNub)
				m_pNub->LockConnecting();
		}
		~CNubConnectingLock()
		{
			if (m_pNub)
				m_pNub->UnlockConnecting();
		}
		void Swap(CNubConnectingLock& lk)
		{
			std::swap(m_pNub, lk.m_pNub);
		}
		CNubConnectingLock& operator=(CNubConnectingLock lk)
		{
			Swap(lk);
			return *this;
		}

	private:
		CNetNub* m_pNub;
	};

	class CNubKeepAliveLock
	{
	public:
		CNubKeepAliveLock() : m_pNub(0) {}
		CNubKeepAliveLock(CNetNub* pNub) : m_pNub(pNub)
		{
			if (m_pNub)
				m_pNub->m_keepAliveLocks++;
		}
		CNubKeepAliveLock(const CNubKeepAliveLock& lk) : m_pNub(lk.m_pNub)
		{
			if (m_pNub)
				m_pNub->m_keepAliveLocks++;
		}
		~CNubKeepAliveLock()
		{
			if (m_pNub)
				m_pNub->m_keepAliveLocks--;
		}
		void Swap(CNubKeepAliveLock& lk)
		{
			std::swap(m_pNub, lk.m_pNub);
		}
		CNubKeepAliveLock& operator=(CNubKeepAliveLock lk)
		{
			Swap(lk);
			return *this;
		}

	private:
		CNetNub* m_pNub;
	};

	void GC_FailedActiveConnect(EDisconnectionCause cause, string description, CNubKeepAliveLock); // game thread

private:
	// someone we're trying to connect to
	struct SPendingConnection : public SKeyExchangeStuff
	{
		string                  connectionString;
		DynArray<uint8>         connectionStringBuf;
		TNetAddress             to;
		TNetAddressVec          tos;
		_smart_ptr<CNetChannel> pChannel;
		CTimeValue              lastSend;
		int                     connectCounter = 0;
		CNubConnectingLock      conlk;
		uint32                  socketCaps = 0;
		CrySessionHandle        session = CrySessionInvalidHandle;

		CNetProfileTokens::EMode tokenMode = CNetProfileTokens::EMode::Legacy;
		CNetProfileTokens::SProfileTokenPair profileTokenPair;
		uint32                  legacyToken = 0;
		ChannelSecurity::CHMac  hmac;
	};

	// someone we're currently connecting with (waiting for game)
	struct SConnecting : public SKeyExchangeStuff
	{
		SConnecting() : lastNotify(0.0f) {}

		uint32           socketCaps;
		CTimeValue       lastNotify;
		CrySessionHandle session;
		CNetProfileTokens::EMode tokenMode = CNetProfileTokens::EMode::Legacy;
		CNetProfileTokens::SProfileTokenPair profileTokenPair;
		ChannelSecurity::CHMac hmac;
	};

	void SendDisconnect(const TNetAddress& to, SDisconnect& dis);
	void AckDisconnect(const TNetAddress& to, ChannelSecurity::CHMac& hmac);

	void LobbySafeDisconnect(CNetChannel* pChannel, EDisconnectionCause cause, const char* reason);

	typedef std::map<TNetAddress, SDisconnect> TDisconnectMap;
	typedef std::set<TNetAddress>              TAckDisconnectSet;
	typedef std::map<TNetAddress, SConnecting> TConnectingMap;
	typedef std::vector<SPendingConnection>    TPendingConnectionSet;

	TPendingConnectionSet::iterator FindPendingConnection(const TNetAddress& from, const bool checkAllTos);

	bool PrepareConnectionString(SPendingConnection& con, ChannelSecurity::CCipher& outputCipher);
	bool ProcessConnectionString(uint8* pBuf, const size_t bufSize, ChannelSecurity::CCipher& inputCipher, string& outConnectionString);

	CNetwork*             m_pNetwork;
	ICryLobby*            m_pLobby;
	SStatistics           m_statistics;
	string                m_hostName;
	TChannelMap           m_channels;
	IDatagramSocketPtr    m_pSocketMain;
	IGameSecurity*        m_pSecurity;
#if ENABLE_GAME_QUERY
	IGameQuery*           m_pGameQuery;
#endif
	IGameNub*             m_pGameNub;
	bool                  m_bDead;
	TNetAddress           m_addr;
	TPendingConnectionSet m_pendingConnections;
	size_t                m_cleanupChannel;
	TDisconnectMap        m_disconnectMap;
	TAckDisconnectSet     m_ackDisconnectSet;
	TConnectingMap        m_connectingMap;
	NetTimerId            m_statsTimer;
	IServerReport*        m_serverReport;
	int                   m_connectingLocks;
	int                   m_keepAliveLocks;
	CryCriticalSection    m_lockChannels;

	bool SendPendingConnect(SPendingConnection& pc);
	void AddDisconnectEntry(const TNetAddress& ip, CrySessionHandle session, const ChannelSecurity::CHMac& hmac, EDisconnectionCause cause, const char* szReason);
	//! Silent disconnect is for cases, when we cannot send disconnect packet (sender is not verified, connection is not established, etc.)
	void AddSilentDisconnectEntry(const TNetAddress& ip, CrySessionHandle session, EDisconnectionCause cause, const char* szReason);

	bool SendKeyExchange0(const TNetAddress& to, SConnecting& con, bool doNotRegenerate);
	bool SendKeyExchange1(SPendingConnection& pc);

	void LockConnecting()
	{
		SCOPED_GLOBAL_LOCK;
		++m_connectingLocks;
	}
	void UnlockConnecting()
	{
		SCOPED_GLOBAL_LOCK;
		--m_connectingLocks;
	}
};

#endif
