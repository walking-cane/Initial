// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_EvaluateTarget.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API UBTService_EvaluateTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_EvaluateTarget();
	
protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere)
	float ScoreToAddCurrentTarget = 500.f;
	
private:
	/** 나중에 Enum 으로 변경해 Random, LowestHealth, Nearest 등 분기를 확장 */
	UPROPERTY(EditAnywhere)
	bool bRandom = false;
};