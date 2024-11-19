// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserWidget.h"

#include "DependencyAnalyserResultRow.h"
#include "DependencyFunctionLibrary.h"
#include "SlateOptMacros.h"
#include "AssetRegistry/AssetData.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

DEFINE_LOG_CATEGORY(LogDependencyAnalyser);

FName SDependencyAnalyserWidget::Name_Name(TEXT("Name"));
FName SDependencyAnalyserWidget::Name_DependenciesCount(TEXT("Dependencies Count"));
FName SDependencyAnalyserWidget::Name_DiskSize(TEXT("Disk Size"));
FName SDependencyAnalyserWidget::Name_MemorySize(TEXT("Memory Size"));
FName SDependencyAnalyserWidget::Name_Type(TEXT("Type"));
FName SDependencyAnalyserWidget::Name_Path(TEXT("Path"));

void SDependencyAnalyserWidget::Construct(const FArguments& InArgs)
{
	UDependencyFunctionLibrary::CacheConfig();
	
	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.FillWidth(1)
			.Padding(5)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SAssignNew(IncludeSoftRef, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Include Soft References"))
					.ToolTipText(FText::FromString("When counting dependencies, include soft references"))
				]
				
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					SAssignNew(IgnoreDevFolders, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Ignore Dev Folders"))
					.ToolTipText(FText::FromString("When counting dependencies and listing assets, ignore dev folders"))
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					SAssignNew(IgnoreExternalActors, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Ignore ExternalActors"))
					.ToolTipText(FText::FromString("When counting dependencies and listing assets, ignore ExternalActors folders"))
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					SAssignNew(IgnoreExternalObjects, SCheckBox)
					.IsChecked(true)
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Ignore ExternalObjects"))
					.ToolTipText(FText::FromString("When counting dependencies and listing assets, ignore ExternalObjects folders"))
				]
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Asset Warning Size (MB):"))
				.ToolTipText(FText::FromString("Assets over this size (in MB) will be marked as warning"))
			]
				
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(WarningSize, SEditableTextBox)
				.Text(FText::AsNumber(UDependencyFunctionLibrary::CachedDefaultWarningSize))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(10, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Asset Error Size (MB):"))
				.ToolTipText(FText::FromString("Assets over this size (in MB) will be marked as error"))
			]
				
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(ErrorSize, SEditableTextBox)
				.Text(FText::AsNumber(UDependencyFunctionLibrary::CachedDefaultErrorSize))
			]
			
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.AutoWidth()
			.Padding(10, 0, 10, 0)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Export as CSV")))
				.ToolTipText(FText::FromString("Exports data (if any is listed) and brings up directory where CSV file is stored (overriding any existing file)"))
				.OnClicked(this, &SDependencyAnalyserWidget::OnExport)
			]
		]
		
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Run Dependency Analysis")))
				.ToolTipText(FText::FromString("Starts running dependency analysis - it might take some time!"))
				.OnClicked(this, &SDependencyAnalyserWidget::OnRun)
			]
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.MaxHeight(30)
		.Padding(5)
		[
			SNew(SSearchBox)
			.HintText(FText::FromString(TEXT("Search Assets by Name or Type...")))
			.OnTextChanged(this, &SDependencyAnalyserWidget::OnSearchBoxChanged)
			.OnTextCommitted(this, &SDependencyAnalyserWidget::OnSearchBoxCommitted)
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.FillHeight(5)
		.Padding(5)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FLineData>>)
			.OnGenerateRow(this, &SDependencyAnalyserWidget::OnGenerateLine)
			.ListItemsSource(&LinesData)
			.ItemHeight(14)
			.Visibility(EVisibility::Visible)
			.HeaderRow(
				SNew(SHeaderRow)
				
				+ SHeaderRow::Column(Name_Name)
				.DefaultLabel(FText::FromName(Name_Name))
				.FillWidth(0.2)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))
				
				+ SHeaderRow::Column(Name_DependenciesCount)
				.DefaultLabel(FText::FromName(Name_DependenciesCount))
				.FillWidth(0.2)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))

				+ SHeaderRow::Column(Name_DiskSize)
				.DefaultLabel(FText::FromName(Name_DiskSize))
				.FillWidth(0.1)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))

				+ SHeaderRow::Column(Name_MemorySize)
				.DefaultLabel(FText::FromName(Name_MemorySize))
				.FillWidth(0.1)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))

				+ SHeaderRow::Column(Name_Type)
				.DefaultLabel(FText::FromName(Name_Type))
				.FillWidth(0.2)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))
				
				+ SHeaderRow::Column(Name_Path)
				.DefaultLabel(FText::FromName(Name_Path))
				.FillWidth(0.3)
				.OnSort(FOnSortModeChanged::CreateSP(this, &SDependencyAnalyserWidget::OnSortColumnHeader))
			)
		]
		
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.Padding(5, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Results"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			]

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.Padding(5, 0, 0, 0)
			[
				SAssignNew(ResultsTextBlock, STextBlock)
			]
		]
	];
}

FReply SDependencyAnalyserWidget::OnRun()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results;
	UDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule, Results);
	UDependencyFunctionLibrary::CachedDefaultWarningSize = FCString::Atoi(*WarningSize.Get()->GetText().ToString());
	UDependencyFunctionLibrary::CachedDefaultErrorSize = FCString::Atoi(*ErrorSize.Get()->GetText().ToString());

	LinesData.Empty();

	int32 ErrorAssetsCount = 0;
	int32 WarningAssetsCount = 0;

	FScopedSlowTask ExplorePathsTask(Results.Num(), FText::FromString("Running..."));
	ExplorePathsTask.MakeDialog();

	for (int32 i = 0; i < Results.Num(); i++)
	{
		ExplorePathsTask.EnterProgressFrame();
		
		if (IgnoreDevFolders->IsChecked() && Results[i].PackageName.ToString().StartsWith("/Game/Developers/"))
		{
			continue;
		}
		if (IgnoreExternalActors->IsChecked() &&
			Results[i].PackageName.ToString().StartsWith("/Game/__ExternalActors__/"))
		{
			continue;
		}
		if (IgnoreExternalObjects->IsChecked() &&
			Results[i].PackageName.ToString().StartsWith("/Game/__ExternalObjects__/"))
		{
			continue;
		}
		if (!UDependencyFunctionLibrary::CachedOnlyAnalyseAssetTypes.IsEmpty() && !UDependencyFunctionLibrary::CachedOnlyAnalyseAssetTypes.Contains(Results[i].GetClass()))
		{
			continue;
		}

		if (Results[i].AssetName.ToString().Contains("L_Expanse"))
		{
			UE_LOG(LogTemp, Log, TEXT(""));
		}

		FAssetData Result = Results[i];
		const FDependenciesData Dependencies = UDependencyFunctionLibrary::GetDependencies(
			AssetRegistryModule,
			Result,
			IncludeSoftRef->IsChecked(),
			IgnoreDevFolders->IsChecked());
		UClass* AssetClass = Result.GetClass();
		FName AssetType = AssetClass ? AssetClass->GetFName() : FName("None");
		FLineData Data = {Result.AssetName.ToString(), Dependencies.Amount, Dependencies.DiskSize, Dependencies.MemorySize, Result.GetAsset(), AssetClass, AssetType, Result.PackageName};

		if (const UBlueprint* InBlueprint = Cast<UBlueprint>(Data.Object))
		{
			Data.Class = InBlueprint->GeneratedClass;
		}

		LinesData.Add(MakeShared<FLineData>(Data));

		if (int32 Size; UDependencyFunctionLibrary::IsErrorDiskSize(Data.Class, Data.DiskSize, Size))
		{
			ErrorAssetsCount++;
		}
		else if (UDependencyFunctionLibrary::IsWarningDiskSize(Data.Class, Data.DiskSize, Size))
		{
			WarningAssetsCount++;
		}

		if (int32 Size; UDependencyFunctionLibrary::IsErrorMemorySize(Data.Class, Data.MemorySize, Size))
		{
			ErrorAssetsCount++;
		}
		else if (UDependencyFunctionLibrary::IsWarningMemorySize(Data.Class, Data.MemorySize, Size))
		{
			WarningAssetsCount++;
		}
	}

	GenerateResultText(LinesData.Num(), ErrorAssetsCount, WarningAssetsCount);

	auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
	{
		return B.Get()->DependenciesCount - A.Get()->DependenciesCount < 0.f;
	};
	LinesData.Sort(ReferenceCountSorter);
	
	LastRunResult = LinesData;

	ListView.Get()->RequestListRefresh();

	return FReply::Handled();
}

FReply SDependencyAnalyserWidget::OnExport()
{
	TArray<FString> Lines;
	for (auto Line : LinesData)
	{
		FString Type = Line.Get()->Class ? Line.Get()->Class->GetName() : FString("None");
		FString Data = FString::Printf(TEXT("%s, %d, %s, %s, %s,"),
			ToCStr(Line.Get()->Name),
			Line.Get()->DependenciesCount,
			ToCStr(UDependencyFunctionLibrary::GetSizeText(Line.Get()->DiskSize).ToString()),
			ToCStr(Type),
			ToCStr(Line.Get()->Path.ToString()));
		Lines.Add(Data);
	}

	const FString Path = FString::Printf(TEXT("DependencyAnalysis/DependencyAnalysisResults_%s.csv"), *FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S")));
	const bool bSuccess = FFileHelper::SaveStringArrayToFile(Lines, *Path);
	
	if (!bSuccess)
	{
		UE_LOG(LogDependencyAnalyser, Error, TEXT("Could not save file to %s"), *Path);
	}
	else
	{
		UE_LOG(LogDependencyAnalyser, Warning, TEXT("Saved to %s"), *Path);
		FWindowsPlatformProcess::ExploreFolder(*Path);
	}
	
	return FReply::Handled();
}

void SDependencyAnalyserWidget::OnSearchBoxChanged(const FText& InSearchText)
{
	if (!InSearchText.IsEmptyOrWhitespace())
	{
		Filter = InSearchText;
	
		RefreshResults();
	}
	else
	{
		LinesData = LastRunResult;
	}
	
	ListView->RequestListRefresh();
}

void SDependencyAnalyserWidget::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	if (!InSearchText.IsEmptyOrWhitespace())
	{
		Filter = InSearchText;
	
		RefreshResults();
	}
	else
	{
		LinesData = LastRunResult;
	}
	
	ListView->RequestListRefresh();
}

void SDependencyAnalyserWidget::RefreshResults()
{
	LinesData.Empty();
	
	for (const auto& LineData : LastRunResult)
	{
		if (DoesPassFilter(LineData))
		{
			LinesData.Add(LineData);
		}
	}
}

void SDependencyAnalyserWidget::GenerateResultText(const int32 TotalAssetsCount, const int32 ErrorAssetsCount, const int32 WarningAssetsCount)
{
	const FString ResultStr = FString::Printf(TEXT("There are %d assets larger than their maximum size and %d assets larger than their recommended size, out of a total of %d assets."),
		ErrorAssetsCount,
		WarningAssetsCount,
		TotalAssetsCount);

	ResultsTextBlock.Get()->SetText(FText::FromString(ResultStr));
}

bool SDependencyAnalyserWidget::DoesPassFilter(const TSharedPtr<FLineData, ESPMode::ThreadSafe>& LineData)
{
	if (LineData.Get()->Name.Contains(Filter.ToString()))
	{
		return true;
	}

	if (LineData.Get()->Class != nullptr && LineData.Get()->Class->GetName().Contains(Filter.ToString()))
	{
		return true;
	}

	return false;
}

TSharedRef<ITableRow> SDependencyAnalyserWidget::OnGenerateLine(TSharedPtr<FLineData> Item, const TSharedRef<STableViewBase> &Table)
{
	int32 Size;
	return SNew(SDependencyAnalyserResultRow, Table)
		.Item(Item)
		.IsWarningSize(UDependencyFunctionLibrary::IsWarningDiskSize(Item->Class, Item->DiskSize, Size))
		.IsErrorSize(UDependencyFunctionLibrary::IsErrorDiskSize(Item->Class, Item->DiskSize, Size))
		.IsWarningMemorySize(UDependencyFunctionLibrary::IsWarningMemorySize(Item->Class, Item->MemorySize, Size))
		.IsErrorMemorySize(UDependencyFunctionLibrary::IsErrorMemorySize(Item->Class, Item->MemorySize, Size));
}

void SDependencyAnalyserWidget::OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode)
{
	if (ColumnName == Name_Name)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->Name > A.Get()->Name;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == Name_DependenciesCount)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->DependenciesCount - A.Get()->DependenciesCount < 0.f;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == Name_DiskSize)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return static_cast<int32>(B.Get()->DiskSize) - static_cast<int32>(A.Get()->DiskSize) < 0.f;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (UDependencyFunctionLibrary::bEnableMemorySizeCalculation && ColumnName == Name_MemorySize)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return static_cast<int32>(B.Get()->MemorySize) - static_cast<int32>(A.Get()->MemorySize) < 0.f;
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == Name_Type)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			if (B.Get()->Class == nullptr || A.Get()->Class == nullptr)
			{
				return false;
			}

			return B.Get()->Class->GetName() > A.Get()->Class->GetName();
		};
		LinesData.Sort(ReferenceCountSorter);
	}
	else if (ColumnName == Name_Path)
	{
		auto ReferenceCountSorter = [](const TSharedPtr<FLineData>& A, const TSharedPtr<FLineData>& B)
		{
			return B.Get()->Path.ToString() > A.Get()->Path.ToString();
		};
		LinesData.Sort(ReferenceCountSorter);
	}

	ListView.Get()->RequestListRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
