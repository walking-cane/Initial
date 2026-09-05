// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelScoreBoardWidget.generated.h"

struct FKernelVerbMessage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalDamageDealt;
	
private:
	FGameplayMessageListenerHandle StatsUpdateListenerHandle;
	
	void OnStatsChanged(FGameplayTag Channel, const FKernelVerbMessage& Message);
};
