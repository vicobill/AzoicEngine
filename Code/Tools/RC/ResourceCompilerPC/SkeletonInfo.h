// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.
//
//  Crytek Engine Source File.
// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.
// -------------------------------------------------------------------------
//  File name:   AnimationInfoLoader.h
//  Version:     v1.00
//  Created:     22/6/2006 by Alexey Medvedev.
//  Compilers:   Visual Studio.NET 2005
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef _SKELETON_INFO
#define _SKELETON_INFO
#pragma once

#include "AnimList.h"
#include <Cry3DEngine/CGF/CGFContent.h>

class CSkeletonInfo
{
public:
	CSkeletonInfo();
	~CSkeletonInfo();

	bool LoadFromChr(const char* name);
	bool LoadFromCga(const char* name);

public:
	CSkinningInfo m_SkinningInfo;
	CAnimList m_animList;
};

#endif
