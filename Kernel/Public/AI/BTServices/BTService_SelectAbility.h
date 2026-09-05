#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BehaviorTree/BTService.h"
#include "AI/KernelAITypes.h"
#include "BTService_SelectAbility.generated.h"

class UAbilitySystemComponent;

/**
 * 적의 상태 태그에 맞는 풀을 골라, 거리·쿨다운·가중치로 어빌리티 하나를 추첨해
 * 블랙보드에 기록한다. 실행은 Task가 담당.
 */
UCLASS()
class KERNEL_API UBTService_SelectAbility : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectAbility();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	/** 상태별 어빌리티 풀. ⚠ 배열 순서가 곧 우선순위 — Rage를 Normal보다 위에 둘 것 */
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (TitleProperty = "StateTag"))
	TArray<FKernelAbilityPool> Pools;

	/** 어느 상태 태그에도 안 걸렸을 때 쓸 풀의 태그 */
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Status.AI"))
	FGameplayTag DefaultStateTag;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChosenActivateKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChosenCompleteKey;

private:
	const FKernelAbilityPool* FindPoolByState(UAbilitySystemComponent* ASC) const;
	const FKernelAbilityPool* FindPoolByTag(const FGameplayTag& StateTag) const;

	static bool CanActivate(UAbilitySystemComponent* ASC, const FGameplayTagContainer& AssetTags);

	/** 직전에 고른 태그 — RepeatPenalty 계산용 */
	FGameplayTag LastChosenTag;
};