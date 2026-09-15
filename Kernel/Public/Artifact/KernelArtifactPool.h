#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KernelArtifactPool.generated.h"

class UKernelArtifactDefinition;

USTRUCT(BlueprintType)
struct FKernelArtifactPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UKernelArtifactDefinition> Artifact;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0"))
	float Weight = 1.f;
};

UCLASS(BlueprintType, Const)
class KERNEL_API UKernelArtifactPool : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FKernelArtifactPoolEntry> Entries;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UKernelArtifactPool>> IncludedPools;
};