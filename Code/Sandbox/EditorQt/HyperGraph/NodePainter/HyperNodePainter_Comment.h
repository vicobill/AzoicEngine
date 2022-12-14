// Copyright 2005-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "IHyperNodePainter.h"

class CHyperNodePainter_Comment : public IHyperNodePainter
{
public:
	virtual void Paint(CHyperNode* pNode, CDisplayList* pList);
};
