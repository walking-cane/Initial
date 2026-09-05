// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelNPC_RollAffix.generated.h"

UCLASS()
class KERNEL_API AKernelNPC_RollAffix : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKernelNPC_RollAffix();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> NPCMesh;

public:
	virtual void OnFocus(APawn* FocusingPawn) override;
	virtual void OnEndFocus(APawn* FocusingPawn) override;
	virtual void OnInteract(APawn* InteractingPawn) override;
};
