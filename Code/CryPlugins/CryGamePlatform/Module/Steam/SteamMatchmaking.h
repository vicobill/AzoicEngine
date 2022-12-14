// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "IPlatformMatchmaking.h"
#include "SteamTypes.h"
#include "SteamUserLobby.h"

namespace Cry
{
	namespace GamePlatform
	{
		namespace Steam
		{
			class CMatchmaking
				: public IMatchmaking
			{
			public:
				explicit CMatchmaking(CService& steamService);
				virtual ~CMatchmaking() = default;

				// IMatchmaking
				virtual void AddListener(IListener& listener, const char* szName) override { m_listeners.Add(&listener, szName); }
				virtual void RemoveListener(IListener& listener) override { m_listeners.Remove(&listener); }

				virtual bool CreateLobby(IUserLobby::EVisibility visibility, int maxMemberCount) override;
				virtual CUserLobby* GetUserLobby(const AccountIdentifier& user) const override;
				virtual CUserLobby* GetUserLobby(const IUser& user) const override;
				virtual CUserLobby* GetLobbyById(const LobbyIdentifier& id) override;

				virtual void QueryLobbies() override;
				virtual LobbyIdentifier GetQueriedLobbyIdByIndex(int index) const override;

				virtual void AddLobbyQueryFilterString(const char* key, const char* value, IUserLobby::EComparison comparison) override;

				virtual void JoinLobby(const LobbyIdentifier& lobbyId) override;
				// ~IMatchmaking

				void OnLobbyRemoved(CUserLobby* pLobby);
			protected:
				CUserLobby* TryGetLobby(const LobbyIdentifier& id);
				CUserLobby* TryGetLobby(const CSteamID& id);

			protected:
				void OnCreateLobby(LobbyCreated_t* pResult, bool bIOFailure);
				CCallResult<CMatchmaking, LobbyCreated_t> m_callResultCreateLobby;

				void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
				CCallResult<CMatchmaking, LobbyMatchList_t> m_callResultLobbyMatchList;

				void OnJoin(LobbyEnter_t* pLobbyMatchList, bool bIOFailure);
				CCallResult<CMatchmaking, LobbyEnter_t> m_callResultLobbyEntered;

				STEAM_CALLBACK(CMatchmaking, OnJoinRequested, GameLobbyJoinRequested_t, m_callbackJoinRequested);
				STEAM_CALLBACK(CMatchmaking, OnGameServerChangeRequested, GameServerChangeRequested_t, m_callbackGameServerChangeRequested);
				STEAM_CALLBACK(CMatchmaking, OnInvited, LobbyInvite_t, m_callbackInvited);

			protected:
				CService& m_service;

				CListenerSet<IListener*> m_listeners = 1;
				std::vector<std::unique_ptr<CUserLobby>> m_lobbies;
			};
		}
	}
}