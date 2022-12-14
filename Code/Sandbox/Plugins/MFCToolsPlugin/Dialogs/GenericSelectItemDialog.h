// Copyright 2006-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "MFCToolsDefines.h"
#include <Util/Variable.h>

class MFC_TOOLS_PLUGIN_API CGenericSelectItemDialog : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CGenericSelectItemDialog)

public:
	typedef enum { eMODE_LIST, eMODE_TREE }   TDialogMode;
	typedef IVariable::IGetCustomItems::SItem SItem;

	CGenericSelectItemDialog(CWnd* pParent = NULL);
	virtual ~CGenericSelectItemDialog();

	void SetTitle(const CString& title)
	{
		m_title = title;
	}

	void ShowDescription(bool bShow)
	{
		m_bShowDesc = bShow;
	}

	virtual CString GetSelectedItem();

	virtual void    PreSelectItem(const CString& name)
	{
		m_preselect = name;
	}

	void SetMode(TDialogMode inMode)
	{
		m_mode = inMode;
	}

	void AllowNew(bool bAllow)
	{
		m_bAllowNew = bAllow;
	}

	void SetTreeSeparator(const CString& sep)
	{
		m_treeSeparator = sep;
	}

	// Override items which are otherwise fetched by GetItems
	void SetItems(const std::vector<CString>& items)
	{
		m_bSet = true;
		m_items.resize(0);
		m_items.reserve(items.size());
		std::vector<CString>::const_iterator iter = items.begin();
		std::vector<CString>::const_iterator iterEnd = items.end();
		SItem item;
		while (iter != iterEnd)
		{
			item.name = item.desc = *iter;
			m_items.push_back(item);
			++iter;
		}
	}

	// Override items which are otherwise fetched by GetItems
	void SetItems(const std::vector<SItem>& items)
	{
		m_bSet = true;
		m_items = items;
	}

protected:
	// Derived Dialogs should override this
	virtual void GetItems(std::vector<SItem>& outItems);

	// Called whenever an item gets selected
	virtual void ItemSelected();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnLbnDoubleClick();
	afx_msg void OnBnClickedNew();
	virtual BOOL OnInitDialog();

	void         ReloadItems();
	void         ReloadTree();

	DECLARE_MESSAGE_MAP()

	CString m_dialogID;
	CString            m_title;
	CString            m_preselect;
	CString            m_selectedItem;
	CString            m_selectedDesc;
	CString            m_treeSeparator;
	CTreeCtrl          m_tree;
	CListBox           m_listBox;
	CImageList         m_imageList;
	CEdit              m_desc;
	std::vector<SItem> m_items;
	TDialogMode        m_mode;
	bool               m_bSet;
	bool               m_bAllowNew;
	bool               m_bShowDesc;
};
