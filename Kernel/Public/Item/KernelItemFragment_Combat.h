
#pragma once

#include "KernelItemFragment.h"
#include "KernelItemFragment_Combat.generated.h"

UCLASS()
class KERNEL_API UKernelItemFragment_Combat : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireDelay = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Progression")
	float MultiplyStatsPerLevel = 5.f;
};