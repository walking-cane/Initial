// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelRewardCrate.generated.h"

class UTimelineComponent;

UCLASS()
class KERNEL_API AKernelRewardCrate : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKernelRewardCrate();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CrateMesh;
	
	virtual void BeginPlay() override;
	void InitializeObject();

public:	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
	void OnWaveCleared();
	
	virtual void OnFocus(APawn* FocusingPawn) override;
	virtual void OnEndFocus(APawn* FocusingPawn) override;
	virtual void OnInteract(APawn* InteractingPawn) override;
	
	virtual FText GetInteractName() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolve();
	
private:
	UPROPERTY() FVector OriginLoc = FVector::ZeroVector;
	float FloatTime = 0.f;
	float FloatHeight = 8.f;
	UPROPERTY(EditAnywhere) float FloatSpeed = 2.f;
	UPROPERTY(EditAnywhere) float RotAmount;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive = false;
	
	UFUNCTION() void OnRep_IsActive();
};
