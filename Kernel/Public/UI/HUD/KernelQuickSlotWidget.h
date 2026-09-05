// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelQuickSlotWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> SlotImage;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> WeaponImage;
	
	UPROPERTY(meta = (BindWidget)) FText MaxAmmo;
	UPROPERTY(meta = (BindWidget)) FText CurrentAmmo;
};
