// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "Tools/BaseTool.h"

namespace Designer
{
class Pivot2BottomTool : public BaseTool
{
public:
	Pivot2BottomTool(EDesignerTool tool) : BaseTool(tool)
	{
	}

	void Enter() override;
};
}
