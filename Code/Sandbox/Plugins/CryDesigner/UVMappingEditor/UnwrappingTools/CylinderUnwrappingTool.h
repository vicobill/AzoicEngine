// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "../BaseTool.h"
#include "Core/UVIsland.h"

namespace Designer {
namespace UVMapping
{
class CylinderUnwrappingTool : public BaseTool
{
public:
	CylinderUnwrappingTool(EUVMappingTool tool) : BaseTool(tool)
	{
	}

	void Enter() override;
};
}
}
