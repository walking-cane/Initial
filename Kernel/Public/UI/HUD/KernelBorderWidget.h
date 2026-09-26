// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelBorderWidget.generated.h"

class UVerticalBox;
class UOverlay;
struct FKernelVerbMessage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelBorderWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FogOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> AngerOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> LetterBox;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FogAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> AngerAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> LetterBoxAnim;
	
	void ToggleBorder(FGameplayTag Channel, const FKernelVerbMessage& Message);
	
private:
	FGameplayMessageListenerHandle BorderListenerHandle;
};
