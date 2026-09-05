// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_BlinkAttack.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UGCN_BlinkAttack : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	virtual bool OnExecute_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dissolve")
	bool bDissolveOut = true;
	
};
