// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRY Stuff ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include <CryCore/Project/CryModuleDefs.h>
#include <CryCore/Platform/platform.h>

#define CRY_USE_MFC
#include <CryCore/Platform/CryAtlMfc.h>

// Notice: we actually need access to the PathUtil namespace with its utility functions, but since CL-1383099, it's broken and
//         causes a compile error when #including <CryString/CryPath.h>.
//         Reason: since that CL ICryPak.h now #includes CryPath.h (and not just vise versa as before), and uses symbols before they had a chance to get declared! -> boom!
//         Workaround: only #include ICryPak.h
#include <CrySystem/File/ICryPak.h>

/////////////////////////////////////////////////////////////////////////////
// STL
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <memory>

/////////////////////////////////////////////////////////////////////////////
// CRY Stuff ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#include <CrySystem/ISystem.h>
#include "Util/EditorUtils.h"
#include <IEditor.h>
#include <CryCore/functor.h>
#include <CryCore/Containers/CryFixedArray.h>
