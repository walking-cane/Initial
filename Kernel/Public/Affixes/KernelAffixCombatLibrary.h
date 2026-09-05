#pragma once

#include "CoreMinimal.h"
#include "KernelAffixCombatLibrary.generated.h"

class UAbilitySystemComponent;
class UKernelItemInstance;

UCLASS()
class KERNEL_API UKernelAffixCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void ApplyAffixOnHit(
		UAbilitySystemComponent* InstigatorASC,
		UAbilitySystemComponent* TargetASC,
		UKernelItemInstance* SourceWeapon, 
		const FHitResult& Hit);
};
