#pragma once

#include "CoreMinimal.h"
#include "Item/KernelItemFragment.h"
#include "KernelItemFragment_Melee.generated.h"

UCLASS()
class KERNEL_API UKernelItemFragment_Melee : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float MeleeRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float MeleeDistance = 150.f;
};