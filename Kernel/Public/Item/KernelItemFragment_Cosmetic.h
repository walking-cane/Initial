
#pragma once

#include "KernelItemFragment.h"
#include "KernelItemFragment_Cosmetic.generated.h"

class AKernelItemPickup;

UCLASS()
class KERNEL_API UKernelItemFragment_Cosmetic : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	TSubclassOf<AKernelItemPickup> PickupActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	TSoftObjectPtr<USkeletalMesh> SpawnMesh;
};