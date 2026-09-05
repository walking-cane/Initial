#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Abilities/GA_FireBase.h"
#include "GA_Fire_Melee.generated.h"

UCLASS()
class KERNEL_API UGA_Fire_Melee : public UGA_FireBase
{
	GENERATED_BODY()
	
protected:
	virtual void Fire() override;
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnMeleeHit(FGameplayEventData Payload);
};