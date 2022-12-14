// Copyright 2016-2021 Crytek GmbH / Crytek Group. All rights reserved.

#include <CrySerialization/Forward.h>

#include <QAbstractItemModel>

class QPropertyTreeLegacy;

class QAbstractItemView;

//! Populate the property tree with the properties of the selected item in a Qt model.
class CModelProperties
{
public:
	struct SSerializer
	{
		Serialization::SStruct m_sstruct;
		std::shared_ptr<void> m_pObject;
	};

	typedef std::function<std::unique_ptr<SSerializer>(QAbstractItemModel* pModel, const QModelIndex& modelIndex)> CreateSerializerFunc;

	explicit CModelProperties(QPropertyTreeLegacy* pPropertyTree);

	void AddCreateSerializerFunc(const CreateSerializerFunc& f);

	void ConnectViewToPropertyObject(QAbstractItemView* pView);
	void DetachPropertyObject();
private:
	void AttachSelectionToPropertyTree(QAbstractItemModel* pModel, const QModelIndexList& indices);

	std::unique_ptr<SSerializer> GetSerializer(QAbstractItemModel* pModel, const QModelIndex& modelIndex);

	QAbstractItemModel* m_pInspectedModel;
	QPropertyTreeLegacy* m_pPropertyTree;
	std::vector<CreateSerializerFunc> m_createSerializerFuncs;
	std::vector<std::shared_ptr<void>> m_serializedObjects;
};
