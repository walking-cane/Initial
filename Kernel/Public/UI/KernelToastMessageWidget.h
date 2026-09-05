// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelToastMessageWidget.generated.h"

struct FKernelToastMessage;
struct FGameplayMessageListenerHandle;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelToastMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ToastMessage;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ToastAnim;
	
	void OnActive(FGameplayTag Channel, const FKernelToastMessage& Message);
	
private:
	FGameplayMessageListenerHandle ToastListenerHandle;
	
	UFUNCTION()
	void HandleAnimFinished();
};
