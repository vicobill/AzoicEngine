// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "Schematyc/Script/IScriptElement.h"

namespace Schematyc
{
struct IScriptImport : public IScriptElementBase<EScriptElementType::Import>
{
	virtual ~IScriptImport() {}

	virtual SGUID GetModuleGUID() const = 0;
};
} // Schematyc
