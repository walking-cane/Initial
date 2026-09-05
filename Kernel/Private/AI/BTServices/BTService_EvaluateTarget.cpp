// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices//BTService_EvaluateTarget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

UBTService_EvaluateTarget::UBTService_EvaluateTarget()
{
	NodeName = TEXT("Evaluate Target");
	
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_EvaluateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!BB || !AICon) return;
	
	APawn* AIPawn = AICon->GetPawn();
	UAIPerceptionComponent* PerceptionComp = AICon->GetPerceptionComponent();
	if (!AIPawn || !PerceptionComp) return;
	
	AActor* CurrentTarget = Cast<AActor>(BB->GetValueAsObject(GetSelectedBlackboardKey()));
	
	TArray<AActor*> SensedActors;
	PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SensedActors);
	
	TArray<AActor*> Candidates;
	Candidates.Reserve(SensedActors.Num());

	for (AActor* Actor : SensedActors)
	{
		if (!Actor || Actor == AIPawn) continue;
		if (!Actor->IsA<AKernelHeroCharacter>()) continue;

		if (UAbilitySystemComponent* TargetASC =
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			if (TargetASC->HasMatchingGameplayTag(TAG_Status_Death_Dying) ||
				TargetASC->HasMatchingGameplayTag(TAG_Status_Death_Dead)) continue;
		}

		Candidates.Add(Actor);
	}

	if (Candidates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EvaluateTarget] 후보 없음 → 타겟 해제"));
		BB->ClearValue(GetSelectedBlackboardKey());
		return;
	}

	AActor* BestTarget = nullptr;

	if (bRandom)
	{
		// 이미 유효한 타겟이 있으면 유지 — 매 틱 바뀌면 적이 갈팡질팡한다
		if (CurrentTarget && Candidates.Contains(CurrentTarget))
		{
			return;
		}

		BestTarget = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	}
	else
	{
		float HighestScore = -TNumericLimits<float>::Max();

		for (AActor* Actor : Candidates)
		{
			float Score = -FVector::Dist(AIPawn->GetActorLocation(), Actor->GetActorLocation());
			if (Actor == CurrentTarget) Score += ScoreToAddCurrentTarget;

			if (Score > HighestScore)
			{
				HighestScore = Score;
				BestTarget   = Actor;
			}
		}
	}

	if (BestTarget && BestTarget != CurrentTarget)
	{
		BB->SetValueAsObject(GetSelectedBlackboardKey(), BestTarget);
	}
}