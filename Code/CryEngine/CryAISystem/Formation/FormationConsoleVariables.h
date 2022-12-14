// Copyright 2019-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CrySystem/ConsoleRegistration.h>

struct SAIConsoleVarsLegacyFormation
{
	void Init();

	DeclareConstIntCVar(FormationSystem, 1);
	DeclareConstIntCVar(DrawFormations, 0);
};
