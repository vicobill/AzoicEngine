// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

class CScrollableWindow : public CWnd
{
public:
	CScrollableWindow();
	virtual ~CScrollableWindow();

	void         SetClientSize(unsigned int nWidth, unsigned int nHeight);

	void         SetAutoScrollWindowFlag(bool boAutoScrollWindow);
protected:
	void         OnClientSizeUpdated();
	void         UpdateScrollBars();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	CRect m_stDesiredClientSize;
	bool m_bHVisible;
	bool m_bVVisible;
	bool m_boShowing;

	bool m_boAutoScrollWindow;
};
