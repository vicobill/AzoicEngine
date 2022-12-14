// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "Controls/PropertiesPanel.h"

class CModelViewport;
class CFacialPreviewDialog;
class CFacialEdContext;

class CFacialPreviewOptionsDialog : public CDialog
{
	DECLARE_DYNAMIC(CFacialPreviewOptionsDialog)
public:

	CFacialPreviewOptionsDialog();
	~CFacialPreviewOptionsDialog();

	void SetViewport(CFacialPreviewDialog* pPreviewDialog);
	void SetContext(CFacialEdContext* pContext);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK()     {}
	virtual void OnCancel() {}
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	CPropertiesPanel*     m_panel;
	CModelViewport*       m_pModelViewportCE;
	CFacialPreviewDialog* m_pPreviewDialog;
	HACCEL                m_hAccelerators;
	CFacialEdContext*     m_pContext;
};
