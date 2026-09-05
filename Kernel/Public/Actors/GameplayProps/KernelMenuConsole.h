// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelMenuConsole.generated.h"

class ULevelSequencePlayer;
class ULevelSequence;
class UKernelInteractionComponent;
class UBoxComponent;

/** 
 *  Server can interact while all players entered overlap space.
 *  Change GamePhase when interacted.
 */ 

UCLASS()
class KERNEL_API AKernelMenuConsole : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:
	AKernelMenuConsole();

	virtual void OnInteract(APawn* InteractingPawn) override;
	virtual void OnFocus(APawn* FocusingPawn) override;
	virtual void OnEndFocus(APawn* FocusingPawn) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ConsoleMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> OverlapCollision;

private:
	UPROPERTY() TSet<TObjectPtr<APlayerState>> GatheredPlayerStates;
	
	int32 CountExistPlayers() const;
	bool CheckAllPlayersGathered() const;
};
