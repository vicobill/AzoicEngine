// Copyright 2018-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "stdafx.h"
#include "UnmuteAllTrigger.h"
#include "TriggerUtils.h"
#include "Common/IImpl.h"

namespace CryAudio
{
//////////////////////////////////////////////////////////////////////////
CUnmuteAllTrigger::~CUnmuteAllTrigger()
{
	CRY_ASSERT_MESSAGE(m_connections.empty(), "There are still connections during %s", __FUNCTION__);
}

//////////////////////////////////////////////////////////////////////////
void CUnmuteAllTrigger::Execute() const
{
	if (!m_connections.empty())
	{
		ExecuteDefaultTriggerConnections(this, m_connections);
	}
	else
	{
		g_pIImpl->UnmuteAll();
	}
}

//////////////////////////////////////////////////////////////////////////
void CUnmuteAllTrigger::AddConnections(TriggerConnections const& connections)
{
	CRY_ASSERT_MESSAGE(m_connections.empty(), "There are still connections during %s", __FUNCTION__);
	m_connections = connections;
}

//////////////////////////////////////////////////////////////////////////
void CUnmuteAllTrigger::Clear()
{
	for (auto const pConnection : m_connections)
	{
		g_pIImpl->DestructTriggerConnection(pConnection);
	}

	m_connections.clear();
}
} // namespace CryAudio
