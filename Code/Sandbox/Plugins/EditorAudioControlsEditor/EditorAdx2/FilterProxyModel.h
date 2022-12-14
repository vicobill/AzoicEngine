// Copyright 2018-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <ProxyModels/AttributeFilterProxyModel.h>

namespace ACE
{
namespace Impl
{
namespace Adx2
{
class CFilterProxyModel final : public QAttributeFilterProxyModel
{
public:

	CFilterProxyModel() = delete;
	CFilterProxyModel(CFilterProxyModel const&) = delete;
	CFilterProxyModel(CFilterProxyModel&&) = delete;
	CFilterProxyModel& operator=(CFilterProxyModel const&) = delete;
	CFilterProxyModel& operator=(CFilterProxyModel&&) = delete;

	explicit CFilterProxyModel(QObject* const pParent)
		: QAttributeFilterProxyModel(QDeepFilterProxyModel::Behavior::AcceptIfChildMatches, pParent)
	{}

	virtual ~CFilterProxyModel() override = default;

private:

	// QAttributeFilterProxyModel
	virtual bool rowMatchesFilter(int sourceRow, QModelIndex const& sourcePparent) const override;
	// ~QAttributeFilterProxyModel

	// QSortFilterProxyModel
	virtual bool lessThan(QModelIndex const& left, QModelIndex const& right) const override;
	// ~QSortFilterProxyModel
};
} // namespace Adx2
} // namespace Impl
} // namespace ACE
