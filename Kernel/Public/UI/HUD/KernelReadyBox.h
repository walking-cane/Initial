// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelReadyBox.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelReadyBox : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetChecked(bool bNewChecked);

protected:
	UPROPERTY(meta = (BindWidget)) 
	TObjectPtr<UImage> CheckBorder;
	UPROPERTY(meta = (BindWidget)) 
	TObjectPtr<UImage> CheckIcon;

	UPROPERTY(Transient, meta = (BindWidgetAnim)) 
	TObjectPtr<UWidgetAnimation> CheckAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim)) 
	TObjectPtr<UWidgetAnimation> UnCheckAnim;

private:
	bool bChecked = false;
};
