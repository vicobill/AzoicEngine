// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"

#include <CrySchematyc/CoreAPI.h>
#include <CryCore/StaticInstanceList.h>
#include "../CoreEnv.h"

namespace Schematyc
{

void RegisterBasicTypes(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_stdModuleGUID);
	{
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(bool));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(int32));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(uint32));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(int64));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(uint64));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(CSharedString));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ObjectId));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ExplicitEntityId));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(CryGUID));
	}
}

} // Schematyc

CRY_STATIC_AUTO_REGISTER_FUNCTION(&Schematyc::RegisterBasicTypes)
