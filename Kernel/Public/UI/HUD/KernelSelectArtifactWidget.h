// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelSelectArtifactWidget.generated.h"

class UHorizontalBox;
class UKernelArtifactEntryWidget;
struct FKernelArtifactOfferMessage;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelSelectArtifactWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKernelArtifactEntryWidget> ArtifactEntryClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> ArtifactEntryBox;
	
	void OfferArtifact(FGameplayTag Channel, const FKernelArtifactOfferMessage& Message);

private:
	FGameplayMessageListenerHandle ListenerHandle;
};
