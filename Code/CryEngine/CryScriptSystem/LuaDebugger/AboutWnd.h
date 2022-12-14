// Copyright 2004-2021 Crytek GmbH / Crytek Group. All rights reserved.

#ifndef __ABOUT_WND_H__
#define __ABOUT_WND_H__

#pragma once

class CAboutWnd : public _TinyDialog
{
public:

	using _TinyWindow::Create;

	BOOL Create(_TinyWindow* pParent = NULL)
	{ return _TinyDialog::Create(MAKEINTRESOURCE(IDD_ABOUTBOX), pParent); };
	BOOL DoModal(_TinyWindow* pParent = NULL)
	{ return _TinyDialog::DoModal(MAKEINTRESOURCE(IDD_ABOUTBOX), pParent); };

protected:
	_BEGIN_DLG_MSG_MAP(_TinyDialog)
	_BEGIN_COMMAND_HANDLER()
	_COMMAND_HANDLER(IDOK, OnOk)
	_END_COMMAND_HANDLER()
	_END_DLG_MSG_MAP()

	LRESULT OnOk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		_TinyVerify(EndDialog(hWnd, 0));
		return 0;
	};
};

#endif
