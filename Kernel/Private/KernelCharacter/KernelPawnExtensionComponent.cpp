// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/KernelPawnExtensionComponent.h"

// Sets default values for this component's properties
UKernelPawnExtensionComponent::UKernelPawnExtensionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKernelPawnExtensionComponent::InitializeExtension(UKernelAbilitySystemComponent* InASC)
{
	if (!InASC) return;
	CachedASC = InASC;
	
	UE_LOG(LogTemp, Warning, TEXT("[PawnExtComp] %s : ASC 캐싱 완료"), *GetOwner()->GetName());
}