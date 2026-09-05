#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KernelAITypes.generated.h"

// KernelAITypes.h
USTRUCT(BlueprintType)
struct KERNEL_API FKernelAbilityChoice
{
	GENERATED_BODY()

	/** 어빌리티를 발동시킬 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag ActivateTag;

	/** Task가 "이제 다음으로 넘어가도 된다"를 판단할 이벤트 태그 (몽타주 노티파이) */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag CompleteEventTag;

	/** 어빌리티 신원 태그 — 쿨다운·차단 여부 확인용 */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer AbilityAssetTags;

	/** 선택 가중치. 클수록 자주, 히든 패턴은 작게 */
	UPROPERTY(EditAnywhere, Category = "Selection", meta = (ClampMin = "0.0"))
	float Weight = 100.f;

	/** 이 거리 범위 안일 때만 후보에 오른다 */
	UPROPERTY(EditAnywhere, Category = "Selection")
	FFloatInterval ValidDistance = FFloatInterval(0.f, 2000.f);

	/** 직전에 쓴 것과 같으면 가중치에 곱할 배율. 0.3이면 연속 사용이 드물어진다 */
	UPROPERTY(EditAnywhere, Category = "Selection", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RepeatPenalty = 0.35f;
};

USTRUCT(BlueprintType)
struct KERNEL_API FKernelAbilityPool
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Pool", meta = (Categories = "Status.AI"))
	FGameplayTag StateTag;
	
	UPROPERTY(EditAnywhere, Category = "Pool", meta = (TitleProperty = "ActivateTag"))
	TArray<FKernelAbilityChoice> Choices;
};