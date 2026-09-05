// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelScreenWarningWidget.generated.h"

struct FKernelVerbMessage;
class UImage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelScreenWarningWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WarnImage;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> WarnAnim;
	
private:
	FGameplayMessageListenerHandle WarnListenerHandle;
	void HandleWarning(FGameplayTag Channel, const FKernelVerbMessage& Message);
	
	UFUNCTION()
	void HandleWarnAnimFinished();
};
