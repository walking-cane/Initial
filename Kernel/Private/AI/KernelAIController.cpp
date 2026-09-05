// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/KernelAIController.h"
#include "NativeGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "KernelCharacter/Enemy/KernelEnemyStateComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_AI_DetectTarget, "AI.Event.DetectTarget");

AKernelAIController::AKernelAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
		
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 8000.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AKernelAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	}
}

void AKernelAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	UKernelEnemyStateComponent* StateComp = GetPawn()->FindComponentByClass<UKernelEnemyStateComponent>();
	
	if (!Actor || !BB || !StateComp) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetPawn()->ActorHasTag("Boss"))
		{
			StateComp->SetEnemyStateTag(FGameplayTag::RequestGameplayTag("State.AI.Normal"));
			UE_LOG(LogTemp, Warning, TEXT("[AIController] 전투 시작! (Combat)"));
		}
	}
	else
	{
		StateComp->SetEnemyStateTag(FGameplayTag::RequestGameplayTag("Status.AI.Idle"));
		UE_LOG(LogTemp, Warning, TEXT("[AIController] 적 놓침! (Idle)"));
	}
}

void AKernelAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (DefaultBehaviorTree)
	{
		RunBehaviorTree(DefaultBehaviorTree);
	}
}