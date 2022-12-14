// Copyright 2007-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __IVOICEPACKETLISTENER_H__
#define __IVOICEPACKETLISTENER_H__

#if !defined(OLD_VOICE_SYSTEM_DEPRECATED)

	#include "VoicePacket.h"
	#include <CryNetwork/INetwork.h>

struct IVoicePacketListener
{
	virtual ~IVoicePacketListener(){}
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual void OnVoicePacket(SNetObjectID object, const TVoicePacketPtr& pPkt) = 0;
};

typedef _smart_ptr<IVoicePacketListener> IVoicePacketListenerPtr;

#endif
#endif
