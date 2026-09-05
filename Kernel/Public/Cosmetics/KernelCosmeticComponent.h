// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "KernelCosmeticComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelCosmeticComponent : public UActorComponent, public IKernelCosmeticInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKernelCosmeticComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ApplyWeaponLayer(TSubclassOf<UAnimInstance> Layer1P, TSubclassOf<UAnimInstance> Layer3P);

	void ChangeWeapon(USkeletalMesh* NewWeaponMesh);
	void PlayEquipMontage(UAnimMontage* Montage);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponLayers)
	TSubclassOf<UAnimInstance> CurrentLayer1P;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponLayers)
	TSubclassOf<UAnimInstance> CurrentLayer3P;
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponMesh)
	TObjectPtr<USkeletalMesh> CurrentWeaponMesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquipMontage)
	TObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(Replicated) // 같은 몽타주 반복 재생 시 OnRep을 확실히 트리거하기 위한 카운터
	uint8 EquipMontageCounter = 0;

	UFUNCTION() void OnRep_EquipMontage();
	UFUNCTION() void OnRep_WeaponMesh();
	UFUNCTION() void OnRep_WeaponLayers();
};
