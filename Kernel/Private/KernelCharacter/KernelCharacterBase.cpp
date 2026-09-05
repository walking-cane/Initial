// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/KernelCharacterBase.h"
#include "KernelCharacter/KernelPawnExtensionComponent.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/AbilitySets/KernelAbilitySet.h"
#include "KernelCharacter/KernelHealthComponent.h"

// Sets default values
AKernelCharacterBase::AKernelCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	KernelPawnExtComp = CreateDefaultSubobject<UKernelPawnExtensionComponent>("PawnExtensionComponent");
	HealthComp = CreateDefaultSubobject<UKernelHealthComponent>("HealthComp");
}

void AKernelCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKernelCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
}

void AKernelCharacterBase::GrantDefaultAbilities()
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	UKernelAbilitySystemComponent* ASC = KernelPawnExtComp->GetKernelAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp,Error,TEXT("ASC is null"))
		return;
	}
	
	if (ASC && AbilitySet)
	{
		AbilitySet->GiveToAbilitySystem(ASC, &GrantedAbilityHandles, this);
	}
}
