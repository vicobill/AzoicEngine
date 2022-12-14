// Copyright 2019-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CrySystem/ConsoleRegistration.h>

struct SAIConsoleVarsMovement
{
	void Init();

	DeclareConstIntCVar(DebugMovementSystem, 0);
	DeclareConstIntCVar(DebugMovementSystemActorRequests, 0);

	int MovementSystemPathReplanningEnabled;
};