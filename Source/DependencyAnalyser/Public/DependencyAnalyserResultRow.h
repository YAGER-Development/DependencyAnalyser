// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "DependencyAnalyserWidget.h"

struct FLineData;

/* Manages the behaviour of each row resulting from a dependency analysis */
class DEPENDENCYANALYSER_API SDependencyAnalyserResultRow final : public SMultiColumnTableRow<TSharedPtr<FLineData>>
{
public:
	
	SLATE_BEGIN_ARGS(SDependencyAnalyserResultRow) {}
	SLATE_ARGUMENT(TSharedPtr<FLineData>, Item)
	SLATE_ARGUMENT(bool, IsWarningSize)
	SLATE_ARGUMENT(bool, IsErrorSize)
	SLATE_ARGUMENT(bool, IsWarningMemorySize)
	SLATE_ARGUMENT(bool, IsErrorMemorySize)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;

private:
	
	FReply OnDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent) const;
	FReply OnClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	void CreateContextMenu(const FVector2D& MousePosition);
	void OpenSizeMap() const;
	void EditAsset() const;
	void BrowseAsset() const;
	
	TSharedPtr<FLineData> Item;
	bool bIsWarningSize = false;
	bool bIsErrorSize = false;
};
