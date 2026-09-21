// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelScoreboardSlotEntry.generated.h"

struct FKernelVerbMessage;
class AKernelPlayerState;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelScoreboardSlotEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetEntry(const AKernelPlayerState* PS);
	void UpdateArtifactEntry(const AKernelPlayerState* PS);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerPortrait;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerWeapon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;
	
	//etc.
	
private:
	FGameplayMessageListenerHandle DamageListenerHandle;
	
	void OnDealtChanged(FGameplayTag Channel, const FKernelVerbMessage& Message);
};
