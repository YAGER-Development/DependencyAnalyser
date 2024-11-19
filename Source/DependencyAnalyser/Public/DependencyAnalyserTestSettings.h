// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/Blueprint.h"
#include "DependencyAnalyserTestSettings.generated.h"

USTRUCT()
struct DEPENDENCYANALYSER_API FAssetLimit
{
	GENERATED_BODY()

	// Dependencies disk size limit in MB
	UPROPERTY(EditAnywhere)
	int32 DiskSizeLimit = 0;
	// Dependencies memory size limit in MB
	UPROPERTY(EditAnywhere)
	int32 MemorySizeLimit = 0;
	// Dependencies reference count
	UPROPERTY(EditAnywhere)
	int32 ReferenceCountLimit = 0;
};

/* Configuration variables to customise plugin behaviour */
UCLASS(config=Engine, defaultconfig)
class DEPENDENCYANALYSER_API UDependencyAnalyserTestSettings : public UObject
{
	GENERATED_BODY()

public:
	// A list of asset types to analyse - leave empty to analyse all assets (might be slow!)
	UPROPERTY(EditAnywhere, config, Category="Settings")
	TArray<UClass*> OnlyAnalyseAssetTypes = { UBlueprint::StaticClass() };
	
	// Whether to include memory size calculation
	UPROPERTY(EditAnywhere, config, Category="Settings")
	bool bEnableMemorySizeCalculation = false;
	
	// Default size at which an asset throws a warning
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningSizeInMB = 50;

	// Default size at which an asset throws an error
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorSizeInMB = 500;
	
	// Default reference count at which an asset throws a warning
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultWarningReferenceCount = 10;

	// Default reference count at which an asset throws an error
	UPROPERTY(EditAnywhere, config, Category="Default Values")
	int32 DefaultErrorReferenceCount = 100;

	// A list of warning sizes and reference counts per asset type that will be used instead of default
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, FAssetLimit> WarningLimitsPerAssetType;
	
	// A list of error sizes and reference counts per asset type that will be used instead of default 
	UPROPERTY(EditAnywhere, config, Category="Type Limits")
	TMap<UClass*, FAssetLimit> ErrorLimitsPerAssetType;
	
	// Whether a test should fail in case a warning is thrown
	UPROPERTY(EditAnywhere, config, Category="Automated Testing")
	bool bFailForWarnings;
};
