// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

uint32 CDeviceTexture::TextureDataSize(uint32 nWidth, uint32 nHeight, uint32 nDepth, int8 nMips, uint32 nSlices, const ETEX_Format eTF, ETEX_TileMode eTM, uint32 eFlags)
{
	// Fallback to the default texture size function
	return CTexture::TextureDataSize(nWidth, nHeight, nDepth, nMips, nSlices, eTF, eTM_None);
}
