// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelScoreboardEntry_Dealt.generated.h"

struct FKernelVerbMessage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelScoreboardEntry_Dealt : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
