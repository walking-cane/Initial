// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "KernelCharacter/KernelCharacterBase.h"
#include "KernelHeroCharacter.generated.h"

class UKernelCharacterDefinition;
class UKernelDamagePopComponent_Niagara;
class UKernelHealthComponent;
class UKernelCosmeticComponent;
class UKernelQuickBarComponent;
class UCameraComponent;
class UKernelWeaponComponent;
class UKernelItemManager;
class UKernelHeroWeaponComponent;
/**
 * 
 */
UCLASS()
class KERNEL_API AKernelHeroCharacter : public AKernelCharacterBase, public IAbilitySystemInterface, public IKernelCosmeticInterface
{
	GENERATED_BODY()
	
public:
	AKernelHeroCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual USkeletalMeshComponent* GetMesh1P() override { return FirstPersonMesh1; }
	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetWeaponMesh1P() override { return WeaponMeshComp1P; }
	virtual USkeletalMeshComponent* GetWeaponMesh3P() override { return WeaponMeshComp3P; }
	
	virtual void OnDeath1P(UAnimMontage* DeathMontage1P) override;
	virtual UAnimMontage* GetDeathMontage() override;
	
	void SetWeaponMesh(USkeletalMesh* NewWeaponMesh);
	
protected:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelHeroComponent> HeroComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelInteractionComponent> InteractionComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelItemManager> ItemManagerComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelQuickBarComponent> QuickBarComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelCosmeticComponent> CosmeticComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelHeroWeaponComponent> WeaponComp;
	
	UPROPERTY(VisibleAnywhere) TObjectPtr<USkeletalMeshComponent> WeaponMeshComp1P;
	UPROPERTY(VisibleAnywhere) TObjectPtr<USkeletalMeshComponent> WeaponMeshComp3P;
	
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UCameraComponent> FirstPersonCamera1;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<USkeletalMeshComponent> FirstPersonMesh1;
	
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UKernelCharacterDefinition> DefaultCharacterDefinition;
	
	void GrantStartingItem();
	
public:
	virtual void OnRep_PlayerState() override;
};
