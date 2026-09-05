// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelMinimalTooltipWidget.generated.h"

class UTextBlock;
class UImage;
struct FKernelInteractionMessage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelMinimalTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractText;

private:
	FGameplayMessageListenerHandle FocusListenerHandle;
	FGameplayMessageListenerHandle EndFocusListenerHandle;

	void OnFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message);
	void OnEndFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message);
};
