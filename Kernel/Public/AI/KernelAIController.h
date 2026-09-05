// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KernelAIController.generated.h"

struct FGameplayTag;
struct FAIStimulus;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class KERNEL_API AKernelAIController : public AAIController
{
	GENERATED_BODY()
public:
	AKernelAIController();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|BT")
	TObjectPtr<UBehaviorTree> DefaultBehaviorTree;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|BT")
	FName TargetActorKeyName = FName("TargetActor");
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;
};
