// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelStageReadyWidget.generated.h"

struct FKernelInteractionMessage;
class UHorizontalBox;
class UKernelReadyBox;
struct FKernelReadyCountMessage;

UCLASS()
class KERNEL_API UKernelStageReadyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget)) 
	TObjectPtr<UHorizontalBox> ReadyBoxContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UKernelReadyBox> ReadyBoxClass;
	
private:
	UPROPERTY() TArray<TObjectPtr<UKernelReadyBox>> ReadyBoxes;
	
	void EnsureBoxCount(int32 Count);
	
	FGameplayMessageListenerHandle StageReadyListenerHandle;
	FGameplayMessageListenerHandle DeActivateListenerHandle;
	
	UFUNCTION()
	void RefreshReadyState(FGameplayTag Channel, const FKernelReadyCountMessage& Message);
	
	UFUNCTION()
	void DeActivateWidget(FGameplayTag Channel, const FKernelInteractionMessage& Message);
};
