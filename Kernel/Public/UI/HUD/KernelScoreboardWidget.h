// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelScoreboardWidget.generated.h"

class UVerticalBox;
class UKernelScoreboardSlotEntry;
struct FKernelVerbMessage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void RefreshEntries(FGameplayTag Channel, const FKernelVerbMessage& Message);
	void OnToggle(FGameplayTag Channel, const FKernelVerbMessage& Message);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKernelScoreboardSlotEntry> EntryClass;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxPlayers = 4;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> BoardEntry;

	UPROPERTY()
	TArray<TObjectPtr<UKernelScoreboardSlotEntry>> EntryPool;
	
private:
	FGameplayMessageListenerHandle ToggleHandle;
	FGameplayMessageListenerHandle RefreshHandle;
};