// Copyright 2012-2021 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 
		Interface for a class that receives events when entities change
		teams
	-------------------------------------------------------------------------
	History:
	- 30:09:2009  : Created by Colin Gulliver

*************************************************************************/

#ifndef _IGAME_RULES_TEAM_CHANGED_LISTENER_H_
#define _IGAME_RULES_TEAM_CHANGED_LISTENER_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include <CryEntitySystem/IEntityBasicTypes.h>

class IGameRulesTeamChangedListener
{
public:
	virtual ~IGameRulesTeamChangedListener() {}

	virtual void OnChangedTeam(EntityId entityId, int oldTeamId, int newTeamId) = 0;
};

#endif // _IGAME_RULES_TEAM_CHANGED_LISTENER_H_
