// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AffixEntryWidget.generated.h"

class UKernelAffixDefinition;
struct FKernelAffixDetail;
class URichTextBlock;
class UTextBlock;
class UImage;
class UButton;

DECLARE_DELEGATE_TwoParams(FOnAffixRerollRequested, int32 /*Index*/, FName /*AffixId*/);

UCLASS()
class KERNEL_API UAffixEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetEntry(const FKernelAffixDetail& Detail, int32 InIndex);
	void SetRerollAllowed(bool bAllowed);
	void ApplyRerollResult(float NewChance, uint8 NewRerollsUsed);
	int32 GetAffixIndex() const { return AffixIndex; }
	
	FOnAffixRerollRequested OnRerollRequested;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> AffixName;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<URichTextBlock> AffixDescription;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> AffixRollButton;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UImage> AffixIcon;

private:
	UFUNCTION()
	void HandleRollClicked();
	void UpdateRollButton();
	
	UPROPERTY() TObjectPtr<const UKernelAffixDefinition> AffixDef;
	
	int32 AffixIndex = INDEX_NONE;
	uint8 RerollsUsed = 0;
	bool bRerollAllowed = false;
};
