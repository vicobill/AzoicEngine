// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "SelectGrowTool.h"

namespace Designer
{
class SelectConnectedTool : public SelectGrowTool
{
public:

	SelectConnectedTool(EDesignerTool tool) : SelectGrowTool(tool)
	{
	}

	void        Enter() override;

	static void SelectConnectedPolygons(MainContext& mc);
};
}
