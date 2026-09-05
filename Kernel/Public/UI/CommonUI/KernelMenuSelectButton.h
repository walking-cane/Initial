// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "KernelMenuSelectButton.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelMenuSelectButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void SetText();
	
protected:
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	virtual void NativeOnClicked() override; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	FText ButtonText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DesiredText; 
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HoveredAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ClickedAnim;
};
