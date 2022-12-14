// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "PropertiesWidget.h"

#include "ComponentsModel.h"
#include "AbstractObjectModel.h"
#include "VariablesModel.h"
#include "ScriptUndo.h"

#include "ScriptBrowserUtils.h"

#include <NodeGraph/AbstractNodeGraphViewModelItem.h>
#include <NodeGraph/NodeGraphUndo.h>

#include <QAdvancedPropertyTreeLegacy.h>
#include <QPropertyTreeLegacy/ContextList.h>
#include <QVBoxLayout>

// TEMP
#include "DetailWidget.h"
// ~TEMP

namespace CrySchematycEditor {

CPropertiesWidget::CPropertiesWidget(CComponentItem& item, CMainWindow* pEditor)
	: m_pEditor(pEditor)
	, m_pPreview(nullptr)
{
	SetupTree();

	m_structs.push_back(Serialization::SStruct(item));
	m_pPropertyTree->attach(m_structs);

	m_pContextList = new Serialization::CContextList();
	m_pPropertyTree->setArchiveContext(m_pContextList->Tail());
}

CPropertiesWidget::CPropertiesWidget(CAbstractObjectStructureModelItem& item, CMainWindow* pEditor)
	: m_pEditor(pEditor)
	, m_pPreview(nullptr)
{
	SetupTree();

	m_structs.push_back(Serialization::SStruct(item));
	m_pPropertyTree->attach(m_structs);

	m_pContextList = new Serialization::CContextList();
	m_pPropertyTree->setArchiveContext(m_pContextList->Tail());
}

CPropertiesWidget::CPropertiesWidget(CAbstractVariablesModelItem& item, CMainWindow* pEditor)
	: m_pEditor(pEditor)
	, m_pPreview(nullptr)
{
	SetupTree();

	m_structs.push_back(Serialization::SStruct(item));
	m_pPropertyTree->attach(m_structs);

	m_pContextList = new Serialization::CContextList();
	m_pPropertyTree->setArchiveContext(m_pContextList->Tail());
}

CPropertiesWidget::CPropertiesWidget(CryGraphEditor::GraphItemSet& items, CMainWindow* pEditor)
	: m_pEditor(pEditor)
	, m_pPreview(nullptr)
{
	SetupTree();

	for (CryGraphEditor::CAbstractNodeGraphViewModelItem* pAbstractItem : items)
	{
		m_structs.push_back(Serialization::SStruct(*pAbstractItem));
		pAbstractItem->SignalDeletion.Connect(this, &CPropertiesWidget::OnContentDeleted);
	}
	m_pPropertyTree->attach(m_structs);

	m_pContextList = new Serialization::CContextList();
	m_pPropertyTree->setArchiveContext(m_pContextList->Tail());
}

void CPropertiesWidget::OnContentDeleted(CryGraphEditor::CAbstractNodeGraphViewModelItem* pDeletedItem)
{
	pDeletedItem->SignalDeletion.DisconnectObject(this);

	m_pPropertyTree->detach();
	auto iter = std::find_if(m_structs.begin(), m_structs.end(), [pDeletedItem](const Serialization::SStruct& a)
		{
			return a.cast<CryGraphEditor::CAbstractNodeGraphViewModelItem>() == pDeletedItem;
		});
	CRY_ASSERT(iter != m_structs.end());
	m_structs.erase(iter);

	if (m_structs.size() > 0)
	{
		m_pPropertyTree->attach(m_structs);
	}
	else
	{
		deleteLater();
	}
}

CPropertiesWidget::CPropertiesWidget(IDetailItem& item, CMainWindow* pEditor, Schematyc::CPreviewWidget* pPreview)
	: m_pDetailItem(&item)
	, m_pContextList(nullptr)
	, m_pEditor(pEditor)
	, m_pPreview(pPreview)
{
	SetupTree();

	m_structs.push_back(Serialization::SStruct(item));
	m_pPropertyTree->attach(m_structs);

	Serialization::CContextList* pContextList = item.GetContextList();
	if (pContextList)
	{
		m_pPropertyTree->setArchiveContext(pContextList->Tail());
	}

	if (m_pPreview)
	{
		QObject::connect(m_pPreview, &Schematyc::CPreviewWidget::signalChanged, this, &CPropertiesWidget::OnPreviewChanged);
	}
}

CPropertiesWidget::~CPropertiesWidget()
{
	if (m_pPreview)
	{
		QObject::disconnect(m_pPreview);
	}
	delete m_pContextList;

	auto iter = std::for_each(m_structs.begin(), m_structs.end(), [this](Serialization::SStruct& a)
		{
			CryGraphEditor::CAbstractNodeGraphViewModelItem* pDeletedItem = a.cast<CryGraphEditor::CAbstractNodeGraphViewModelItem>();
			if (pDeletedItem)
				pDeletedItem->SignalDeletion.DisconnectObject(this);
		});
}

void CPropertiesWidget::SetupTree()
{
	QVBoxLayout* pLayout = new QVBoxLayout(this);

	m_pPropertyTree = new QAdvancedPropertyTreeLegacy("Component Properties");
	m_pPropertyTree->setExpandLevels(2);
	m_pPropertyTree->setValueColumnWidth(0.6f);
	m_pPropertyTree->setAggregateMouseEvents(false);
	m_pPropertyTree->setFullRowContainers(true);
	m_pPropertyTree->setSizeToContent(true);
	// Disable use of actions / buttons on preview entity
	m_pPropertyTree->setActionsEnabled(false);

	PropertyTreeStyle treeStyle(QPropertyTreeLegacy::defaultTreeStyle());
	treeStyle.propertySplitter = false;
	m_pPropertyTree->setTreeStyle(treeStyle);

	QObject::connect(m_pPropertyTree, &QPropertyTreeLegacy::signalBeginUndo, this, &CPropertiesWidget::OnBeginUndo);
	QObject::connect(m_pPropertyTree, &QAdvancedPropertyTreeLegacy::signalChanged, this, &CPropertiesWidget::OnPropertiesChanged);
	QObject::connect(m_pPropertyTree, &QPropertyTreeLegacy::signalEndUndo, this, &CPropertiesWidget::OnEndUndo);

	pLayout->addWidget(m_pPropertyTree);
}

void CPropertiesWidget::OnPropertiesChanged()
{
	SignalPropertyChanged();

	if (m_pPropertyTree)
	{
		m_pPropertyTree->revertNoninterrupting();
	}
}

void CPropertiesWidget::OnPreviewChanged()
{
	if (m_pPropertyTree)
	{
		m_pPropertyTree->revertNoninterrupting();
	}
}

void CPropertiesWidget::OnBeginUndo()
{
	if (m_pEditor)
	{
		CScriptUndoObject* pUndoObject = new CScriptUndoObject("Properties modified.", *m_pEditor);
		m_latestUndoActionDescription = pUndoObject->GetDescription();

		GetIEditor()->GetIUndoManager()->Begin();
		GetIEditor()->GetIUndoManager()->RecordUndo(pUndoObject);
	}
}

void CPropertiesWidget::OnEndUndo(bool acceptUndo)
{
	if (m_pEditor)
	{
		if (acceptUndo)
		{
			GetIEditor()->GetIUndoManager()->Accept(m_latestUndoActionDescription);
		}
		else
		{
			GetIEditor()->GetIUndoManager()->Cancel();
		}
	}
}

void CPropertiesWidget::showEvent(QShowEvent* pEvent)
{
	QWidget::showEvent(pEvent);

	if (m_pPropertyTree)
		m_pPropertyTree->setSizeToContent(true);
}

}
