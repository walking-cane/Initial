#include "AI/BTTasks/BTTask_ActivateAbilityByTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("Activate Ability By Tag");

	bCreateNodeInstance = true;

	TargetKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, TargetKey), AActor::StaticClass());
	ChosenActivateKey.AddNameFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, ChosenActivateKey));
	ChosenCompleteKey.AddNameFilter(this,
		GET_MEMBER_NAME_CHECKED(ThisClass, ChosenCompleteKey));
}

bool UBTTask_ActivateAbilityByTag::ResolveTags(
	UBlackboardComponent* BB, FGameplayTag& OutActivate, FGameplayTag& OutComplete) const
{
	// 경로 1 : 에디터에서 직접 지정한 태그
	if (OverrideActivateTag.IsValid())
	{
		OutActivate = OverrideActivateTag;
		OutComplete = OverrideCompleteTag;
		return true;
	}

	// 경로 2 : Service가 블랙보드에 써둔 태그
	if (!BB) return false;

	const FName ActivateName = BB->GetValueAsName(ChosenActivateKey.SelectedKeyName);
	if (ActivateName.IsNone())
	{
		// Service가 아직 못 골랐거나, 지금 쓸 수 있는 어빌리티가 없음
		return false;
	}

	// 두 번째 인자 false = 미등록 태그일 때 크래시 대신 빈 태그 반환
	OutActivate = FGameplayTag::RequestGameplayTag(ActivateName, false);
	if (!OutActivate.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BTTask] 등록되지 않은 태그: %s"), *ActivateName.ToString());
		return false;
	}

	const FName CompleteName = BB->GetValueAsName(ChosenCompleteKey.SelectedKeyName);
	OutComplete = CompleteName.IsNone()
		? FGameplayTag()
		: FGameplayTag::RequestGameplayTag(CompleteName, false);

	return true;
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!BB || !AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	UAbilitySystemComponent* ASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
	if (!ASC) return EBTNodeResult::Failed;

	FGameplayTag ActivateTag;
	FGameplayTag CompleteTag;
	if (!ResolveTags(BB, ActivateTag, CompleteTag)) return EBTNodeResult::Failed;

	FGameplayEventData Payload;
	Payload.Instigator = AIPawn;
	Payload.Target     = TargetActor;

	// 완료 이벤트가 없으면 발동만 하고 즉시 종료
	if (!CompleteTag.IsValid())
	{
		const int32 Fired = ASC->HandleGameplayEvent(ActivateTag, &Payload);
		return Fired > 0 ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	CachedOwnerComp   = &OwnerComp;
	CachedASC         = ASC;
	ActiveCompleteTag = CompleteTag;

	// 이벤트를 쏘기 전에 구독
	CompleteHandle = ASC->AddGameplayEventTagContainerDelegate(
		FGameplayTagContainer(CompleteTag),
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
			this, &ThisClass::HandleCompleteEvent));

	const int32 Triggered = ASC->HandleGameplayEvent(ActivateTag, &Payload);

	if (Triggered <= 0)
	{
		Cleanup();
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_ActivateAbilityByTag::HandleCompleteEvent(
	FGameplayTag Tag, const FGameplayEventData* EventData)
{
	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;

	Cleanup();

	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::AbortTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (CachedASC && !CancelAbilityTags.IsEmpty())
	{
		// BT가 이 분기를 버리면 어빌리티도 취소
		CachedASC->CancelAbilities(&CancelAbilityTags);
	}

	Cleanup();
	return EBTNodeResult::Aborted;
}

void UBTTask_ActivateAbilityByTag::Cleanup()
{
	if (CachedASC && CompleteHandle.IsValid() && ActiveCompleteTag.IsValid())
	{
		CachedASC->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(ActiveCompleteTag), CompleteHandle);
	}

	CompleteHandle.Reset();
	ActiveCompleteTag = FGameplayTag();
	CachedASC         = nullptr;
	CachedOwnerComp   = nullptr;
}