// Copyright 2012-2021 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 
		Interface for a class that received events when items are picked up
	-------------------------------------------------------------------------
	History:
	- 23:09:2009  : Created by Colin Gulliver

*************************************************************************/

#ifndef _IGAME_RULES_PICKUP_LISTENER_H_
#define _IGAME_RULES_PICKUP_LISTENER_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include <CryEntitySystem/IEntityBasicTypes.h>

class IGameRulesPickupListener
{
public:
	virtual ~IGameRulesPickupListener() {}

	virtual void OnItemPickedUp(EntityId itemId, EntityId actorId) = 0;
	virtual void OnItemDropped(EntityId itemId, EntityId actorId) = 0;

	virtual void OnPickupEntityAttached(EntityId entityId, EntityId actorId) = 0;
	virtual void OnPickupEntityDetached(EntityId entityId, EntityId actorId, bool isOnRemove) = 0;
};

#endif // _IGAME_RULES_PICKUP_LISTENER_H_
