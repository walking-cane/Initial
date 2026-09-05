// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KernelCrossHairStyle.h"
#include "Blueprint/UserWidget.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CrosshairWidget.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class KERNEL_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTargetStyle(const FKernelCrosshairStyle& NewStyle) { TargetStyle = NewStyle; }

	/** 발사 시 순간적으로 벌어지는 양 (반동 연동) */
	void AddSpread(float Amount) { CurrentSpread += Amount; }
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void ApplyStyle(const FKernelCrosshairStyle& Style, float ExtraGap);
	void OnActiveItemChanged(FGameplayTag, const FKernelQuickBarActiveIndexChangedMessage& Message);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Top;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Bottom;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Left;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Right;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CenterDot;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Ring;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float InterpSpeed = 12.f;      // 무기 전환 속도

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float SpreadRecoverySpeed = 8.f;

private:
	FGameplayMessageListenerHandle CrosshairListenerHandle;
	
	FKernelCrosshairStyle CurrentStyle;
	FKernelCrosshairStyle TargetStyle;
	FKernelCrosshairStyle UnarmedStyle;
	
	float CurrentSpread = 0.f;
};
