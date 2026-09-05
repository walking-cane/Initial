// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelPawnExtensionComponent.generated.h"

class UKernelAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelPawnExtensionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKernelPawnExtensionComponent();

	FORCEINLINE UKernelAbilitySystemComponent* GetKernelAbilitySystemComponent() const
	{
		return CachedASC;
	}
	
	void InitializeExtension(UKernelAbilitySystemComponent* InASC);
	
protected:
	UPROPERTY()
	TObjectPtr<UKernelAbilitySystemComponent> CachedASC;
};
