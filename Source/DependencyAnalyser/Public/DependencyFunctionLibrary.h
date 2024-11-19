// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DependencyFunctionLibrary.generated.h"

struct DEPENDENCYANALYSER_API FDependenciesData
{
	int32 Amount;
	SIZE_T DiskSize;
	SIZE_T MemorySize;
};

/* Public function library to be used by editor tool and test suite */
UCLASS()
class DEPENDENCYANALYSER_API UDependencyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	// Retrieves all assets to analyse
	static void RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule, TArray<FAssetData>& AssetData);

	// Collects and returns dependencies from an internal recursive call
	static FDependenciesData GetDependencies(const FAssetRegistryModule& AssetRegistryModule, const FAssetData& AssetData,
		const bool IncludeSoftReferences, const bool IgnoreDevFolders);

	// Inform about whether an asset is over a specified size or ref count
	static bool IsWarningDiskSize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize);
	static bool IsWarningMemorySize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize);
	static bool IsWarningCount(const UClass* Class, const int32 Count, int32& OutWarningCount);
	static bool IsErrorDiskSize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize);
	static bool IsErrorMemorySize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize);
	static bool IsErrorCount(const UClass* Class, const int32 Count, int32& OutErrorCount);
	static bool IsOverMBSize(const SIZE_T Size, const int32 SizeMB);

	static FText GetSizeText(const SIZE_T SizeInBytes);

	// Retrieval and caching of config data
	static void CacheConfig();
	inline static int32 CachedDefaultWarningSize = 50;
	inline static int32 CachedDefaultErrorSize = 500;
	inline static int32 CachedDefaultWarningCount = 10;
	inline static int32 CachedDefaultErrorCount = 100;
	inline static TArray<UClass*> CachedOnlyAnalyseAssetTypes;
	inline static bool bCachedFailForWarnings;
	inline static bool bEnableMemorySizeCalculation;

private:
	
	// Collects dependencies recursively
	static void GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule, const FName& PackageName,
		const UE::AssetRegistry::EDependencyQuery QueryType, const bool IgnoreDevFolders, TArray<FName>& OutDependencies);

	// Retrieved and cached config data
	inline static TMap<UClass*, int32> CachedWarningDiskSizePerType;
	inline static TMap<UClass*, int32> CachedErrorDiskSizePerType;
	inline static TMap<UClass*, int32> CachedWarningMemorySizePerType;
	inline static TMap<UClass*, int32> CachedErrorMemorySizePerType;
	inline static TMap<UClass*, int32> CachedWarningCountPerType;
	inline static TMap<UClass*, int32> CachedErrorCountPerType;
};
