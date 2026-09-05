// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelInteractionComponent.generated.h"

enum class EKernelGamePhase : uint8;
class IKernelInteractionInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKernelInteractionComponent();

protected:
	FTimerHandle InteractionTimerHandle;
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionEnabled(bool bEnabled);
	
	TWeakObjectPtr<AActor> CurrentTargetActor;
	IKernelInteractionInterface* CurrentTarget = nullptr;
	
private:
	void TraceForInteraction();
	
	void BroadcastFocus(IKernelInteractionInterface* Target);
	void BroadcastEndFocus();
	
	void TryBindToGameState();
	void HandleGamePhase(EKernelGamePhase Phase);
};
