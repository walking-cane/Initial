// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GameplayEvent.generated.h"

UCLASS()
class KERNEL_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EventTag;
};
