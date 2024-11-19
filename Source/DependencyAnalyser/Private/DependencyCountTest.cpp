// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyFunctionLibrary.h"
#include "AssetRegistry/AssetData.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FDependencyCountTest, "DependencyAnalyser.DependencyCountTest",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FDependencyCountTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	UDependencyFunctionLibrary::CacheConfig();
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results;
	UDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule, Results);
	for (int32 i = 0; i < Results.Num(); i++)
	{
		const FString& Filename = Results[i].PackageName.ToString();
		OutBeautifiedNames.Add(FPaths::GetBaseFilename(Filename));
		OutTestCommands.Add(Filename);
	}
}

bool FDependencyCountTest::RunTest(const FString& Parameters)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FAssetData MockAssetData;
	MockAssetData.PackageName = *Parameters;
	
	const FDependenciesData Dependencies = UDependencyFunctionLibrary::GetDependencies(
		AssetRegistryModule,
		MockAssetData,
		true,
		true);

	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPackageName(*Parameters, AssetData, true);

	if (!AssetData.IsEmpty())
	{
		if (int32 ErrorCount; UDependencyFunctionLibrary::IsErrorCount(AssetData[0].GetClass(), Dependencies.Amount, ErrorCount))
		{
			const FString& ErrorMsg = FString::Printf(TEXT("Asset %s of reference count %d is larger than max %d reference count"), *Parameters, Dependencies.Amount, ErrorCount); 
			AddError(ErrorMsg);
			return false;
		}

		if (int32 WarningCount; UDependencyFunctionLibrary::IsWarningCount(AssetData[0].GetClass(), Dependencies.Amount, WarningCount))
		{
			const FString& WarningMsg = FString::Printf(TEXT("Asset %s of reference count %d is larger than recommended %d reference count"), *Parameters, Dependencies.Amount, WarningCount); 

			if (UDependencyFunctionLibrary::bCachedFailForWarnings)
			{
				AddError(WarningMsg);
				return false;
			}
		
			AddWarning(WarningMsg);
			return true;
		}
	}
	
	return true;
}
