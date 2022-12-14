// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "Target.h"
#include <CrySerialization/Math.h>

CRY_PFX2_DBG

namespace pfx2
{

CTargetSource::CTargetSource(ETargetSource source)
	: m_offset(ZERO)
	, m_source(source)
{
}

void CTargetSource::Serialize(Serialization::IArchive& ar)
{
	ar(m_source, "Source", "^");
	ar(m_offset, "Offset", "Offset");
}

void CTargetSource::AddToComponent(CParticleComponent* pComponent)
{
	if (m_source == ETargetSource::Target)
		pComponent->AddEnvironFlags(ENV_TARGET);
}

}
