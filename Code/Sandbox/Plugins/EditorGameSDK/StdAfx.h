// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#include <CryCore/Platform/platform.h>

#define CRY_USE_MFC
#define CRY_SUPPRESS_CRYENGINE_WINDOWS_FUNCTION_RENAMING
#define CRY_USE_XT
#include <CryCore/Platform/CryAtlMfc.h>

#include "Resource.h"

#include <CryCore/functor.h>
#include <CryCore/smartptr.h>
#include <CryMath/Cry_Geo.h>
#include <CryExtension/CryGUID.h>

// STL headers.
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
