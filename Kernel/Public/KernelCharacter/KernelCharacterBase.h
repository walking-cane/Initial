// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelCharacterBase.generated.h"

class UKernelHealthComponent;
class UGameplayAbility;
class UKernelAttributeSet;
class UKernelInteractionComponent;
struct FGameplayAbilitySpecHandle;
class UKernelHeroComponent;
class UKernelAbilitySet;
class UKernelPawnExtensionComponent;
class UKernelAbilitySystemComponent;
class UKernelInputConfig;

UCLASS()
class KERNEL_API AKernelCharacterBase : public ACharacter, 
	public IKernelInteractionInterface
{
	GENERATED_BODY()

public:
	AKernelCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;


protected:
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelHealthComponent> HealthComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelPawnExtensionComponent> KernelPawnExtComp;
	
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UKernelAbilitySet> AbilitySet;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(Transient) TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	
	void GrantDefaultAbilities();
};