// Copyright 2013-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CrySerialization/Forward.h>

inline bool Serialize(Serialization::IArchive & ar, class CCryName & cryName, const char* name, const char* label);

#include "CryNameImpl.h"
