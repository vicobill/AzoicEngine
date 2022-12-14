// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

class CUIEnumerations
{
public:
	// For XML standard values.
	typedef std::vector<string>        TDValues;
	typedef std::map<string, TDValues> TDValuesContainer;

	static CUIEnumerations& GetUIEnumerationsInstance();

	TDValuesContainer&      GetStandardNameContainer();
};
