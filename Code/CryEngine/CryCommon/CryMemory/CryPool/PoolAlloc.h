// Copyright 2010-2021 Crytek GmbH / Crytek Group. All rights reserved.

// Created by: Michael Kopietz
// Modified: -
//
//---------------------------------------------------------------------------

#ifndef __CCRYPOOLALLOC__
#define __CCRYPOOLALLOC__

#if defined(POOLALLOCTESTSUIT)
//cheat just for unit testing on windows
	#include <CryCore/BaseTypes.h>
	#define ILINE inline
#endif

#if CRY_PLATFORM_POSIX
	#define CPA_ALLOC memalign
	#define CPA_FREE  free
#else
	#define CPA_ALLOC _aligned_malloc
	#define CPA_FREE  _aligned_free
#endif
#define CPA_ASSERT  assert

#if CRY_PLATFORM_APPLE
ILINE void* memalign(size_t nAlign, size_t nSize)
{
	void* pBlock(NULL);
	posix_memalign(&pBlock, nSize, nAlign);
	return pBlock;
}
#endif // CRY_PLATFORM_APPLE

#include "List.h"
#include "Memory.h"
#include "Container.h"
#include "Allocator.h"
#include "Defrag.h"
#include "STLWrapper.h"
#include "Inspector.h"
#include "Fallback.h"
#if !defined(POOLALLOCTESTSUIT)
	#include "ThreadSafe.h"
#endif

#undef CPA_ASSERT

#endif
