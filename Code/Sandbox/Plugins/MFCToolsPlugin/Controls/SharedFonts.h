// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once
#include "MFCToolsDefines.h"

struct MFC_TOOLS_PLUGIN_API SMFCFonts
{
	//! Use instead of gMFCFonts. Will come in handy for MFC plugins
	static SMFCFonts& GetInstance();

	SMFCFonts();
	HFONT hSystemFont;         // Default system GUI font.
	HFONT hSystemFontBold;     // Default system GUI bold font.
	HFONT hSystemFontItalic;   // Default system GUI italic font.
	int   nDefaultFontHieght;  // Default font height for 8 logical units.
};

extern SMFCFonts gMFCFonts;
