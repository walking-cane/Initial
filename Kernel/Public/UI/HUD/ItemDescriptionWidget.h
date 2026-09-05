// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/KernelItemTypes.h"
#include "Messages/KernelInteractionMessage.h"
#include "ItemDescriptionWidget.generated.h"

class UButton;
class UTextBlock;
struct FGameplayTag;
class UImage;
class UAffixEntryWidget;
/**
 * 
 */
UCLASS()
class KERNEL_API UItemDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ItemName;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ItemDescription;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> ItemImage;
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UPanelWidget> AffixEntryBox;
	
private:
	FGameplayMessageListenerHandle FocusListenerHandle;
	FGameplayMessageListenerHandle EndFocusListenerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAffixEntryWidget> AffixEntryClass;
	
	void OnFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message);
	void OnEndFocus(FGameplayTag Channel, const FKernelInteractionMessage& Message);
	void HandleAffixRerolled(int32 AffixIndex, const FKernelAppliedAffix& NewApplied);
};
