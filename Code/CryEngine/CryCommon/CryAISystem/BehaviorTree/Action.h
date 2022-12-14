// Copyright 2012-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "Node.h"

namespace BehaviorTree
{
class Action : public Node
{
	typedef Node BaseClass;

public:
	virtual void HandleEvent(const EventContext& context, const Event& event) override
	{
	}

	virtual LoadResult LoadFromXml(const XmlNodeRef& xml, const struct LoadContext& context, const bool isLoadingFromEditor) override
	{
		IF_UNLIKELY (BaseClass::LoadFromXml(xml, context, isLoadingFromEditor) == LoadFailure)
			return LoadFailure;

		if (xml->getChildCount() == 0)
		{
			return LoadSuccess;
		}
		else
		{
			ErrorReporter(*this, context).LogError("No children allowed but found %d.", xml->getChildCount());
			return LoadFailure;
		}
	}

	virtual void OnInitialize(const UpdateContext& context) override
	{
	}

	virtual void OnTerminate(const UpdateContext& context) override
	{
	}
};
}
