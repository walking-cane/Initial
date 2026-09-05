// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelBlockEntrance.generated.h"

/** 
 *  스테이지 중간중간의 진행을 막는 문.
 *  모든 플레이어가 상호작용 할 시 사라짐.
 *  Replicate 되나, 상호작용 이펙트는 로컬로 발송 (OnRep)
*/

UCLASS()
class KERNEL_API AKernelBlockEntrance : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AKernelBlockEntrance();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnInteract(APawn* InteractingPawn) override;
	virtual void OnFocus(APawn* FocusingPawn) override;
	virtual void OnEndFocus(APawn* FocusingPawn) override;
	
	void TryToOpenDoor();
	void OpenDoor();
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BlockMesh;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void PlayOpenSequence();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void FinishOpen();
	
private:
	/** 문 열림과 동시에 재생할 CameraShake */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> OpeningCameraShake;
	
	UPROPERTY(ReplicatedUsing = OnRep_GatheredPlayerStates)
	TArray<TObjectPtr<APlayerState>> GatheredPlayerStates;
	
	int32 CountExistPlayers() const;
	
	UPROPERTY(ReplicatedUsing = OnRep_Opening)
	bool bOpening = false;
	
	bool bIsReady = false;
	
	UFUNCTION()
	void OnRep_GatheredPlayerStates();
	
	UFUNCTION()
	void OnRep_Opening();
};
