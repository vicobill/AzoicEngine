// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "SampleAssetType.h"

#include <AssetSystem/AssetEditor.h>
#include <PathUtils.h>

REGISTER_ASSET_TYPE(CSampleAssetType)

namespace Private_SampleAssetType
{

// For the sake of simplicity, we directly use the C file API in this sample.
// In a real-world scenario, however, you should prefer the ICryPak interface.

bool CreateDataFile(const char* szFilePath)
{
	FILE* pFile;
	if (fopen_s(&pFile, szFilePath, "w"))
	{
		return false;
	}
	fprintf(pFile, "hello, world");
	fclose(pFile);
	return true;
}

} // namespace Private_SampleAssetType

bool CSampleAssetType::OnCreate(INewAsset& asset, const SCreateParams* pCreateParams) const
{
	using namespace Private_SampleAssetType;

	const string basePath = PathUtil::RemoveExtension(PathUtil::RemoveExtension(asset.GetMetadataFile()));

	const string dataFilePath = basePath + ".txt";
	const string absoluteDataFilePath = PathUtil::Make(PathUtil::GetGameProjectAssetsPath(), dataFilePath);

	if (!CreateDataFile(absoluteDataFilePath))
	{
		return false;
	}

	asset.AddFile(dataFilePath);

	return true;
}

CAssetEditor* CSampleAssetType::Edit(CAsset* pAsset) const
{
	return CAssetEditor::OpenAssetForEdit("Sample Asset Editor", pAsset);
}
