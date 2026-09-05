// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemLog.h"
#include "EnhancedInputComponent.h"
#include "KernelCharacter/Input/KernelInputConfig.h"
#include "KernelInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;

UCLASS()
class KERNEL_API UKernelInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	UKernelInputComponent(const FObjectInitializer& ObjectInitializer);
	
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UKernelInputConfig* InputConfig,UserClass* Object, 
		PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,TArray<uint32>& BindHandles);
	
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UKernelInputConfig* InputConfig, const FGameplayTag& InputTag, 
		ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);
};

//Implement template func
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UKernelInputComponent::BindAbilityActions(const UKernelInputConfig* InputConfig,UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,TArray<uint32>& BindHandles)
{
	check(InputConfig);
	
	for (const FKernelInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(
					Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag).GetHandle());
				UE_LOG(LogAbilitySystem, Warning, TEXT("Binding PressedFunc %s to %s"), *Action.InputTag.ToString(), *GetOwner()->GetName());
			}
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(
					Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
				UE_LOG(LogAbilitySystem, Warning, TEXT("Binding ReleasedFunc %s to %s"), *Action.InputTag.ToString(), *GetOwner()->GetName());
			}
		}
	}
}

template<class UserClass, typename FuncType>
void UKernelInputComponent::BindNativeAction(const UKernelInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}