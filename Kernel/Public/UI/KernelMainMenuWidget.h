// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelMainMenuWidget.generated.h"

class UCommonButtonBase;
/**
 * 
 */

enum class EMenuAction : uint8
{
	None,
	Start,
	Join,
	Quit
};

UCLASS()
class KERNEL_API UKernelMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void OnMenuButtonClicked(UCommonButtonBase* Button);
	void OnMenuButtonHovered(UCommonButtonBase* Button);

	void HandleStart();
	void HandleJoin();
	void HandleQuit();

	UFUNCTION()
	void OnFadeOutFinished();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> MenuTheme;
	UPROPERTY()
	TObjectPtr<UAudioComponent> CachedAudioComp;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> StartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> JoinButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> QuitButton;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> SelectFadeAnimation;
	
private:
	EMenuAction PendingAction = EMenuAction::None;
};
