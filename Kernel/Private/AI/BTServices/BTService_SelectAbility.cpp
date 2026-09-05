#include "AI/BTServices/BTService_SelectAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SelectAbility::UBTService_SelectAbility()
{
	NodeName = TEXT("Select Ability");

	// LastChosenTag를 적마다 따로 들기 위해 필수
	bCreateNodeInstance = true;

	Interval        = 0.3f;
	RandomDeviation = 0.05f;

	TargetKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, TargetKey), AActor::StaticClass());
	ChosenActivateKey.AddNameFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, ChosenActivateKey));
	ChosenCompleteKey.AddNameFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, ChosenCompleteKey));
}

void UBTService_SelectAbility::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!BB || !AICon) return;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target)
	{
		BB->ClearValue(ChosenActivateKey.SelectedKeyName);
		BB->ClearValue(ChosenCompleteKey.SelectedKeyName);
		return;
	}

	UAbilitySystemComponent* ASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
	if (!ASC) return;

	const FKernelAbilityPool* Pool = FindPoolByState(ASC);
	if (!Pool) Pool = FindPoolByTag(DefaultStateTag);
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[SelectAbility] 풀을 찾지 못함 — Pools %d개"), Pools.Num());
		return;
	}

	const float Dist = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());

	TArray<const FKernelAbilityChoice*> Candidates;
	TArray<float> Weights;
	float TotalWeight = 0.f;

	for (const FKernelAbilityChoice& Choice : Pool->Choices)
	{
		if (!Choice.ActivateTag.IsValid()) continue;
		if (Choice.Weight <= 0.f) continue;

		// 거리 조건
		if (Dist < Choice.ValidDistance.Min || Dist > Choice.ValidDistance.Max) continue;

		// 쿨다운·차단은 GAS에게
		if (!CanActivate(ASC, Choice.AbilityAssetTags)) continue;

		float W = Choice.Weight;
		if (Choice.ActivateTag == LastChosenTag)
		{
			W *= Choice.RepeatPenalty;
		}
		if (W <= 0.f) continue;

		Candidates.Add(&Choice);
		Weights.Add(W);
		TotalWeight += W;
	}

	if (Candidates.Num() == 0 || TotalWeight <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SelectAbility] 후보 0개 — Dist=%.0f, Pool=%s"),
			Dist, *Pool->StateTag.ToString());
		BB->ClearValue(ChosenActivateKey.SelectedKeyName);
		BB->ClearValue(ChosenCompleteKey.SelectedKeyName);
		return;
	}

	// ── 가중 추첨 ──
	float Pick = FMath::FRand() * TotalWeight;
	const FKernelAbilityChoice* Chosen = Candidates.Last();

	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		Pick -= Weights[i];
		if (Pick <= 0.f)
		{
			Chosen = Candidates[i];
			break;
		}
	}

	LastChosenTag = Chosen->ActivateTag;

	BB->SetValueAsName(ChosenActivateKey.SelectedKeyName, Chosen->ActivateTag.GetTagName());
	BB->SetValueAsName(ChosenCompleteKey.SelectedKeyName,
		Chosen->CompleteEventTag.IsValid() ? Chosen->CompleteEventTag.GetTagName() : NAME_None);
}

const FKernelAbilityPool* UBTService_SelectAbility::FindPoolByState(UAbilitySystemComponent* ASC) const
{
	if (!ASC) return nullptr;

	// 배열 순서 = 우선순위. 여러 상태 태그가 동시에 붙어도 위에 있는 것이 이긴다
	for (const FKernelAbilityPool& P : Pools)
	{
		if (P.StateTag.IsValid() && ASC->HasMatchingGameplayTag(P.StateTag))
		{
			return &P;
		}
	}
	return nullptr;
}

const FKernelAbilityPool* UBTService_SelectAbility::FindPoolByTag(const FGameplayTag& StateTag) const
{
	if (!StateTag.IsValid()) return nullptr;

	for (const FKernelAbilityPool& P : Pools)
	{
		if (P.StateTag == StateTag) return &P;
	}
	return nullptr;
}

bool UBTService_SelectAbility::CanActivate(
	UAbilitySystemComponent* ASC, const FGameplayTagContainer& AssetTags)
{
	if (AssetTags.IsEmpty()) return true;   // 태그 미지정이면 검사 생략

	TArray<FGameplayAbilitySpec*> Specs;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AssetTags, Specs);

	for (const FGameplayAbilitySpec* Spec : Specs)
	{
		if (!Spec || !Spec->Ability) continue;

		if (Spec->Ability->CanActivateAbility(Spec->Handle, ASC->AbilityActorInfo.Get()))
		{
			return true;
		}
	}
	return false;
}