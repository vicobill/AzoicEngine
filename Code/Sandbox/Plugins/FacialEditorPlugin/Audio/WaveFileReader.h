// Copyright 2005-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CrySystem/IStreamEngine.h>
#include "Util/MemoryBlock.h"

class CWaveFileReader
{
public:

	CWaveFileReader();
	~CWaveFileReader();

	bool  LoadFile(const char* sFileName);
	int32 GetSample(uint32 nPos);
	float GetSampleNormalized(uint32 nPos);
	void  GetSamplesMinMax(int nPos, int nSamplesCount, float& vmin, float& vmax);
	//int32	GetLengthMs() const { return m_pSoundbufferInfo.nLengthInMs; }
	bool  IsLoaded()              { return m_bLoaded; }
	void  SetLoaded(bool bLoaded) { m_bLoaded = bLoaded; }

	//uint32	GetSampleCount() { return m_pSoundbufferInfo.nLengthInSamples; }
	//uint32	GetSamplesPerSec() { return m_pSoundbufferInfo.nBaseFreq; }
	//uint32	GetBitDepth() { return m_pSoundbufferInfo.nBitsPerSample; }

	//bool GetSoundBufferInfo(SSoundBufferInfo* pSoundInfo);

protected:
	void* LoadAsSample(const char* AssetDataPtrOrName, int nLength);

	// Closes down Stream or frees memory of the Sample
	void DestroyData();

	CMemoryBlock m_MemBlock;
	bool         m_bLoaded;
	//SSoundBufferInfo	m_pSoundbufferInfo;
	uint32       m_nVolume;
};
