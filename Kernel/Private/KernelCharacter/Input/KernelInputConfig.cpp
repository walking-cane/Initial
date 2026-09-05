// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/Input/KernelInputConfig.h"

const UInputAction* UKernelInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FKernelInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
