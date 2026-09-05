// KernelItemDefinition.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KernelItemDefinition.generated.h"

class UKernelItemFragment;

UCLASS(BlueprintType, Const)
class KERNEL_API UKernelItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 이 아이템을 구성하는 기능 파편(Fragment)들의 배열입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced, meta = (NoResetToDefault))
	TArray<TObjectPtr<UKernelItemFragment>> Fragments;

	const UKernelItemFragment* FindFragmentByClass(TSubclassOf<UKernelItemFragment> FragmentClass) const;
	
	UPROPERTY(EditDefaultsOnly) FText DisplayName;
	UPROPERTY(EditDefaultsOnly) FText Description;
	UPROPERTY(EditDefaultsOnly) TSoftObjectPtr<UTexture2D> Icon;
};