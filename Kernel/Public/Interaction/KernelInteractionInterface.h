// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Messages/KernelInteractionMessage.h"
#include "KernelInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKernelInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KERNEL_API IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:
	virtual void OnInteract(APawn* InteractingPawn) {}
	virtual void OnFocus(APawn* FocusingPawn) {}
	virtual void OnEndFocus(APawn* FocusingPawn) {}
	
	virtual TSubclassOf<UUserWidget> GetInteractWidgetClass() { return nullptr; }
	virtual FText GetInteractName() { return FText::GetEmpty(); }
	virtual FText GetInteractDescription() { return FText::GetEmpty(); }
	virtual UTexture2D* GetInteractIcon() { return nullptr; }
	virtual TArray<FKernelAffixDetail> GetInteractDetails() { return {}; }
	virtual FVector GetInteractLocation() { return FVector::ZeroVector; }
}; 

