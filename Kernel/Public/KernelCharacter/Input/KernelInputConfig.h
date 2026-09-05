// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "KernelInputConfig.generated.h"

enum class ETriggerEvent : uint8;
class UInputAction;

USTRUCT(BlueprintType)
struct FKernelInputAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS()
class KERNEL_API UKernelInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FKernelInputAction> AbilityInputActions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FKernelInputAction> NativeInputActions;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
};
