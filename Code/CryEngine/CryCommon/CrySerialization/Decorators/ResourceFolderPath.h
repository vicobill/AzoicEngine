// Copyright 2013-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CrySerialization/Forward.h>

namespace Serialization
{

struct ResourceFolderPath
{
	string* path;
	string  startFolder;

	explicit ResourceFolderPath(string& path, const char* startFolder = "")
		: path(&path)
		, startFolder(startFolder)
	{
	}

	virtual ~ResourceFolderPath() {}

	//! The function should stay virtual to ensure cross-dll calls are using right heap.
	virtual void SetPath(const char* path) { *this->path = path; }
};

bool Serialize(Serialization::IArchive& ar, Serialization::ResourceFolderPath& value, const char* name, const char* label);

}

#include "ResourceFolderPathImpl.h"
