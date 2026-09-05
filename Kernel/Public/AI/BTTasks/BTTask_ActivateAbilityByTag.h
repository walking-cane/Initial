#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ActivateAbilityByTag.generated.h"

class UAbilitySystemComponent;
struct FGameplayEventData;

/**
 * 어빌리티를 이벤트로 발동시키고, 완료 이벤트가 올 때까지 BT를 붙잡아둔다.
 *
 * 태그를 얻는 방법이 두 가지다.
 *  - Override 태그를 지정하면 그걸 사용 (오프닝 패턴처럼 고정 실행용)
 *  - 비워두면 SelectAbility 서비스가 블랙보드에 써둔 값을 사용
 */
UCLASS()
class KERNEL_API UBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbilityByTag();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** 지정하면 블랙보드 대신 이 태그로 발동한다 */
	UPROPERTY(EditAnywhere, Category = "Ability|Override")
	FGameplayTag OverrideActivateTag;

	/** Override 사용 시의 완료 이벤트. 비우면 발동 즉시 Succeeded */
	UPROPERTY(EditAnywhere, Category = "Ability|Override")
	FGameplayTag OverrideCompleteTag;

	/** 중단 시 취소할 어빌리티들의 공통 AssetTag (예: Ability.AI.Kallari) */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer CancelAbilityTags;

	/** 공격 대상 (Object 키) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	/** Service가 고른 발동 태그가 담긴 Name 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChosenActivateKey;

	/** Service가 고른 완료 태그가 담긴 Name 키 */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ChosenCompleteKey;

private:
	/** 이번 실행에 쓸 태그 두 개를 결정한다. 실패하면 false */
	bool ResolveTags(UBlackboardComponent* BB, FGameplayTag& OutActivate, FGameplayTag& OutComplete) const;

	void HandleCompleteEvent(FGameplayTag Tag, const FGameplayEventData* EventData);
	void Cleanup();

	UPROPERTY(Transient) TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp = nullptr;
	UPROPERTY(Transient) TObjectPtr<UAbilitySystemComponent> CachedASC = nullptr;

	FDelegateHandle CompleteHandle;

	/** 구독 해제할 때 필요 — 실행 중에만 유효 */
	FGameplayTag ActiveCompleteTag;
};