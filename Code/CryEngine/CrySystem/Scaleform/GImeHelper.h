// Copyright 2015-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once
#include <CrySystem/Scaleform/ConfigScaleform.h>
#if defined(INCLUDE_SCALEFORM3_SDK) && defined(USE_GFX_IME)
	#include "GFxIMEManager.h"
	#include <CrySystem/IWindowMessageHandler.h>

	#if CRY_PLATFORM_WINDOWS
// Note: This file is part of the IME plug-in for Scaleform, and requires linking against the GFxIME.lib
		#include "GFxIMEManagerWin32.h"
typedef GFxIMEManagerWin32 GFxImeManagerBase;
	#else
		#error No IME implementation on this platform
	#endif

// Helper for Scaleform IME
class GImeHelper : public GFxImeManagerBase, public IWindowMessageHandler
{
public:
	GImeHelper();
	~GImeHelper();
	bool ApplyToLoader(GFxLoader* pLoader);
	bool ForwardEvent(GFxIMEEvent& event);
	void SetImeFocus(GFxMovieView* pMovie, bool bSet);

	#if CRY_PLATFORM_WINDOWS
	// IWindowMessageHandler
	void PreprocessMessage(CRY_HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool HandleMessage(CRY_HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	// ~IWindowMessageHandler
	#endif

private:
	// Get the current movie
	GFxMovieView* m_pCurrentMovie;
};

#endif //defined(INCLUDE_SCALEFORM3_SDK) && defined(USE_GFX_IME)
