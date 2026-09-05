#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Abilities/GA_FireBase.h"
#include "GA_Fire_Projectile.generated.h"

UCLASS()
class KERNEL_API UGA_Fire_Projectile : public UGA_FireBase
{
	GENERATED_BODY()
	
protected:
	virtual void Fire() override;
};