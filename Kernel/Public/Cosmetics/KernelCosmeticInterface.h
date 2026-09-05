// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KernelCosmeticInterface.generated.h"

UINTERFACE(MinimalAPI)
class UKernelCosmeticInterface : public UInterface
{
	GENERATED_BODY()
};

class KERNEL_API IKernelCosmeticInterface
{
	GENERATED_BODY()

public:
	virtual USkeletalMeshComponent* GetMesh1P() { return nullptr; }
	virtual USkeletalMeshComponent* GetWeaponMesh1P() { return nullptr; }
	virtual USkeletalMeshComponent* GetWeaponMesh3P() { return nullptr; }
	virtual UAnimMontage* GetDeathMontage() { return nullptr; };
	virtual void OnDeath1P(UAnimMontage* DeathMontage) { return; };
};
