// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once
#include "EditorFramework/Editor.h"
#include "Controls/EditorDialog.h"
#include "SubstanceCommon.h"
#include <QSplitter>

class CInspectorLegacy;
namespace SubstanceAir
{
struct RenderResult;
}

namespace EditorSubstance
{
namespace Renderers
{
struct SPreviewGeneratedOutputData;
}

namespace OutputEditor
{
class CGraphViewModel;

class CSubstanceOutputEditorWidget : public QWidget
{
	Q_OBJECT;
public:
	CSubstanceOutputEditorWidget(const std::vector<SSubstanceOutput>& originalOutputs, const std::vector<SSubstanceOutput>& customOutputs, bool showPreviews, QWidget* pParent = nullptr);
	void             LoadUpdatedOutputSettings(std::vector<SSubstanceOutput>& outputs);
	CGraphViewModel* GetGraphViewModel() { return m_pGraphViewModel.get(); }
private:
	std::unique_ptr<CGraphViewModel> m_pGraphViewModel;
	QSplitter*                       m_pSplitter;
	CInspectorLegacy*                m_pInspector;
};

}
}
