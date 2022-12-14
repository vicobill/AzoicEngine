// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "SmartObjectEditorPlugin.h"

#include <CryCore/Platform/platform_impl.inl>

#include <CrySandbox/CryFunction.h>

#include "Controls/PropertyItem.h"

#include "SmartObjectClassDialog.h"
#include "SmartObjectActionDialog.h"
#include "SmartObjectEventDialog.h"
#include "SmartObjectHelperDialog.h"
#include "SmartObjectPatternDialog.h"
#include "SmartObjectTemplateDialog.h"
#include "SmartObjectStateDialog.h"

#include <Objects/ObjectManager.h>
#include <Objects/EntityObject.h>

#include <CryAISystem/IAISystem.h>
#include <CryAISystem/IAgent.h>

REGISTER_PLUGIN(CSmartObjectEditorPlugin);

CSmartObjectEditorPlugin::CSmartObjectEditorPlugin()
{
	typedef std::function<bool(const string&, string&)> EditPropertyFunc;

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOClass, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOClass, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOState, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOState, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOStates, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOStates, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOStatePattern, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOStatePattern, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOAction, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOAction, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOHelper, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOHelper, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySONavHelper, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySONavHelper, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOAnimHelper, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOAnimHelper, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOEvent, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOEvent, oldValue, newValue);
	}));

	GetIEditor()->RegisterDeprecatedPropertyEditor(ePropertySOTemplate, EditPropertyFunc([this](const string& oldValue, string& newValue)
	{
		return OnEditDeprecatedProperty(ePropertySOTemplate, oldValue, newValue);
	}));
}

CSmartObjectEditorPlugin::~CSmartObjectEditorPlugin()
{

}

void CSmartObjectEditorPlugin::OnEditorNotifyEvent(EEditorNotifyEvent event)
{
	switch (event)
	{
	case eNotify_OnExportToGame:
		ValidateSmartObjects();
		break;
	}
}

void CSmartObjectEditorPlugin::ValidateSmartObjects() const
{
	CryLog("Validating SmartObjects");

	static CObjectClassDesc* pClass = GetIEditor()->GetObjectManager()->FindClass("SmartObject");
	CRY_ASSERT_MESSAGE(pClass != nullptr, "SmartObject class desc not found");
	//this code now assumes SmartObjects are declared and are inherited from CEntityObject.h.
	//See SmartObject.h in the SmartObjectsEditor plugin where it will now reside.

	string error;
	CBaseObjectsArray objects;
	GetIEditor()->GetObjectManager()->GetObjects(objects);

	CBaseObjectsArray::iterator it, itEnd = objects.end();
	for (it = objects.begin(); it != itEnd; ++it)
	{
		CBaseObject* pBaseObject = *it;
		if (pBaseObject->GetClassDesc() == pClass)
		{
			CEntityObject* pSOEntity = (CEntityObject*)pBaseObject;

			if (!gEnv->pAISystem->GetSmartObjectManager()->ValidateSOClassTemplate(pSOEntity->GetIEntity()))
			{
				const Vec3 pos = pSOEntity->GetWorldPos();

				CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, "SmartObject '%s' at (%.2f, %.2f, %.2f) is invalid!",
					(const char*)pSOEntity->GetName(), pos.x, pos.y, pos.z);
			}
		}
	}
}

bool CSmartObjectEditorPlugin::OnEditDeprecatedProperty(int type, const string& oldValue, string& newValue) const
{
	switch (type)
	{
	case ePropertySOClass:
	{
		CSmartObjectClassDialog soDlg;
		soDlg.SetSOClass(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOClass();
			return true;
		}
		return false;
	}
	case ePropertySOClasses:
	{
		CSmartObjectClassDialog soDlg(nullptr, true);
		soDlg.SetSOClass(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOClass();
			return true;
		}
		return false;
	}
	case ePropertySOState:
		//intended fallthrough
	case ePropertySOStates:
	{
		// true = allow multiple smart-object states (careful: this is a quickfix to prevent a crash, as in single-state mode there's the CTreeCtrl missing)
		CSmartObjectStateDialog soDlg(nullptr, true);
		soDlg.SetSOState(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOState();
			return true;
		}
		return false;
	}
	case ePropertySOStatePattern:
	{
		CString sPattern = oldValue;
		CSmartObjectPatternDialog soPatternDlg;
		if (sPattern.Find('|') < 0)
		{
			CSmartObjectStateDialog soDlg(nullptr, true, true);
			soDlg.SetSOState(CString(oldValue));
			switch (soDlg.DoModal())
			{
			case IDOK:
				newValue = soDlg.GetSOState();
				return true;
			case IDCANCEL:
				return false;
			case IDCONTINUE:
				sPattern = soDlg.GetSOState();
				break;
			}
		}
		soPatternDlg.SetPattern(sPattern);
		if (soPatternDlg.DoModal() == IDOK)
		{
			newValue = soPatternDlg.GetPattern();
			return true;
		}
		return false;
	}
	case ePropertySOAction:
	{
		CSmartObjectActionDialog soDlg(nullptr);
		soDlg.SetSOAction(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOAction();
			return true;
		}
		return false;
	}
	case ePropertySOHelper:
	{
		CSmartObjectHelperDialog soDlg(nullptr, true, false, false);
		int f = oldValue.find(':');
		if (f > 0)
		{
			soDlg.SetSOHelper(CString(oldValue.Left(f)), CString(oldValue.Mid(f + 1)));
			if (soDlg.DoModal() == IDOK)
			{
				newValue = soDlg.GetSOHelper();
				return true;
			}
		}
		else
		{
			CQuestionDialog::SCritical(QObject::tr(""), QObject::tr("This field can not be edited because it needs the smart object class.\nPlease, choose a smart object class first..."));
		}
		return false;
	}
	case ePropertySONavHelper:
	{
		CSmartObjectHelperDialog soDlg(nullptr, false, true, false);
		int f = oldValue.find(':');
		if (f > 0)
		{
			soDlg.SetSOHelper(CString(oldValue.Left(f)), CString(oldValue.Mid(f + 1)));
			if (soDlg.DoModal() == IDOK)
			{
				newValue = soDlg.GetSOHelper();
				return true;
			}
		}
		else
		{
			CQuestionDialog::SCritical(QObject::tr(""), QObject::tr("This field can not be edited because it needs the smart object class.\nPlease, choose a smart object class first..."));
		}

		return false;
	}
	case ePropertySOAnimHelper:
	{
		CSmartObjectHelperDialog soDlg(nullptr, true, false, true);
		int f = oldValue.find(':');
		if (f > 0)
		{
			soDlg.SetSOHelper(CString(oldValue.Left(f)), CString(oldValue.Mid(f + 1)));
			if (soDlg.DoModal() == IDOK)
			{
				newValue = soDlg.GetSOHelper();
				return true;
			}
		}
		else
		{
			CQuestionDialog::SCritical(QObject::tr(""), QObject::tr("This field can not be edited because it needs the smart object class.\nPlease, choose a smart object class first..."));
		}
		return false;
	}
	case ePropertySOEvent:
	{	
		CSmartObjectEventDialog soDlg;
		soDlg.SetSOEvent(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOEvent();
			return true;
		}
		return false;
	}
	case ePropertySOTemplate:
	{
		CSmartObjectTemplateDialog soDlg;
		soDlg.SetSOTemplate(CString(oldValue));
		if (soDlg.DoModal() == IDOK)
		{
			newValue = soDlg.GetSOTemplate();
			return true;
		}
		return false;
	}
	default:
		CRY_ASSERT(0); //not implemented
	}

	return false;
}
