// Copyright 2004-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

class CDynamicArray2D
{
public:
	CDynamicArray2D(unsigned int iDimension1, unsigned int iDimension2);
	~CDynamicArray2D();

	void ScaleImage(CDynamicArray2D* pDestination);
	void GetMemoryUsage(ICrySizer* pSizer);

	float** m_Array;

private:
	unsigned int m_Dimension1;
	unsigned int m_Dimension2;
};
