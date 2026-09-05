// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelHeroComponent.generated.h"

struct FInputActionValue;
class UKernelAbilitySystemComponent;
class UKernelPawnExtensionComponent;
class UInputMappingContext;
struct FGameplayTag;
class UKernelInputConfig;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKernelHeroComponent();
	
	virtual void BeginPlay() override;
	
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
	void Input_SwapSlot(const FInputActionValue& InputActionValue);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_MouseLook(const FInputActionValue& InputActionValue);
	void Input_Inventory();
	void Input_ScoreBoard();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UKernelInputConfig> InputConfig;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY()
	TObjectPtr<UKernelPawnExtensionComponent> PawnExtComp;
	UPROPERTY()
	TObjectPtr<UKernelAbilitySystemComponent> KernelASC;
	
	void Input_AbilityInputPressed(FGameplayTag InputTag);
	void Input_AbilityInputReleased(FGameplayTag InputTag);
	
	UPROPERTY()
	bool bIsInputCompleted;
};