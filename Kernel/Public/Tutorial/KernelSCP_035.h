// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbility/AbilitySets/KernelAbilitySet.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelSCP_035.generated.h"

class UKernelLetterBox;
class UKernelAbilitySet;
class UGameplayEffect;
class ALevelSequenceActor;

UCLASS()
class KERNEL_API AKernelSCP_035 : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKernelSCP_035();
	
	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetSadMesh() { return SadMesh; }
	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetHappyMesh() { return HappyMesh1; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DefaultRoot;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SadMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> HappyMesh1;
	
public:
	virtual void OnFocus(APawn* FocusingPawn) override;
	virtual void OnEndFocus(APawn* FocusingPawn) override;
	virtual void OnInteract(APawn* InteractingPawn) override;
	
	UFUNCTION() 
	void OnSequenceFinished();
	
	UFUNCTION(BlueprintCallable)
	void StartCameraFade(bool bReverse, float FadeDuration);
	
	UFUNCTION(BlueprintCallable) void SendFogEvent();
	UFUNCTION(BlueprintCallable) void SendAngerEvent();
	UFUNCTION(BlueprintCallable) void SendFadeOutEvent();
	
	UFUNCTION(BlueprintCallable) void StartCameraShake();
	UFUNCTION(BlueprintCallable) void StopCameraShake();
	
	UFUNCTION(BlueprintCallable) void StartLetterBox();
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<ALevelSequenceActor> LevelSequence;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShake;
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<APlayerController> CachedPC;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UKernelAbilitySet> GrantAbilitySet;
	
	FKernelAbilitySet_GrantedHandles GrantedHandles;
	
	bool bPlayed;
};
