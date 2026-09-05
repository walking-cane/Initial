// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayLayout.generated.h"

struct FGameplayTag;
struct FKernelVerbMessage;
class UAffixRollWidget;
class UCrosshairWidget;
class UKernelToastMessageWidget;
class UKernelStageReadyWidget;
class UKernelMinimalTooltipWidget;
class UKernelQuickSlotWidget;
class UItemDescriptionWidget;
/**
 * 
 */
UCLASS()
class KERNEL_API UGameplayLayout : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairWidget> CrossHair; 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UKernelQuickSlotWidget> QuickSlot; 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemDescriptionWidget> ItemDescription;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UKernelMinimalTooltipWidget> MinimalTooltip;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UKernelStageReadyWidget> StageReadyWidget1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UKernelToastMessageWidget> CommonWidget;
	
private:
	FGameplayMessageListenerHandle ToggleListenerHandle;
	void ToggleGameplayLayout(FGameplayTag Channel, const FKernelVerbMessage& Message);
};
