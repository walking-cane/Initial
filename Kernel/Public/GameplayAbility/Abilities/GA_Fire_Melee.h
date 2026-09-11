#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/Abilities/GA_FireBase.h"
#include "GA_Fire_Melee.generated.h"

UCLASS()
class KERNEL_API UGA_Fire_Melee : public UGA_FireBase
{
	GENERATED_BODY()
	
public:
	virtual void InputReleased(
    		const FGameplayAbilitySpecHandle Handle,
    		const FGameplayAbilityActorInfo* ActorInfo, 
    		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	virtual void Fire() override;
	virtual void StartFireLoop() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FName> ComboSections;
	
	int32 ComboIndex = 0;

	bool IsInputHeld() const;
	
	UFUNCTION() void OnComboWindow(FGameplayEventData Payload);
	
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageCancelled();
	UFUNCTION() void OnMeleeHit(FGameplayEventData Payload);
};