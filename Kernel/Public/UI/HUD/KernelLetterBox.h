// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelLetterBox.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UKernelLetterBox : public UUserWidget
{
	GENERATED_BODY()

public:
	
	
protected:
	UPROPERTY(meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LetterBoxIn;
};
