// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Item/KernelItemTypes.h"
#include "KernelAffixDefinition.generated.h"

class UKernelAbilitySet;
class UGameplayEffect;

UENUM()
enum class EAffixApplyType : uint8
{
	Chance,
	Proportion,
};

USTRUCT(BlueprintType)
struct FKernelChanceStyleRule
{
	GENERATED_BODY()

	/** 이 값 이상이면 아래 스타일 적용 (내림차순으로 정렬해서 넣을 것) */
	UPROPERTY(EditDefaultsOnly) float MinPercent = 0.f;

	/** RichText 스타일 데이터 테이블의 Row Name */
	UPROPERTY(EditDefaultsOnly) FName StyleName = FName("ChanceGreen");
};

UCLASS()
class KERNEL_API UKernelAffixDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/** 세이브·식별용 영구 ID. */
	UPROPERTY(EditDefaultsOnly, Category = "Identity")
	FName AffixId;

	UPROPERTY(EditDefaultsOnly, Category = "Identity")
	FText DisplayName;

	/** 툴팁 문구. {0}에 롤된 확률(%)이 치환된다 — "공격 시 {0}% 확률로 화상" */
	UPROPERTY(EditDefaultsOnly, Category = "Identity")
	FText DescriptionFormat;

	UPROPERTY(EditDefaultsOnly, Category = "Identity")
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	EItemRarity MinRarity = EItemRarity::Normal;

	/** 풀에서 뽑힐 가중치. 높을수록 자주 등장 */
	UPROPERTY(EditDefaultsOnly, Category = "Roll", meta = (ClampMin = "0.01"))
	int32 PickWeight = 1;
	
	/** 켜면 전역 등급 구간 대신 아래 구간을 사용 */
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	bool bOverrideChanceRange = false;

	UPROPERTY(EditDefaultsOnly, Category = "Roll",
		meta = (EditCondition = "bOverrideChanceRange"))
	TArray<FKernelRarityChanceRange> ChanceRangeOverride;
	
	// ── 실제 효과 ──
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	EAffixApplyType ApplyType = EAffixApplyType::Chance;
	
	/** 명중 시 확률 통과하면 '피격 대상'에게 적용할 GE (화상 DoT, 슬로우 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> OnHitEffectToTarget;

	/** 명중 시 '자신'에게 적용할 GE (흡혈 회복 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> OnHitEffectToSelf;

	/** 확률 통과 시 자신의 ASC로 보낼 이벤트 태그 — 이 태그를 AbilityTrigger로 가진 GA가 발동 */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGameplayTag TriggerEventTag;   // 예: Event.Affix.AttackTwice

	/**
	 * TriggerEventTag로 발동될 어빌리티를 담은 세트.
	 * 무기 장착 시 부여되고 해제 시 회수된다 (기존 EquipFrag->GrantAbilitySet과 동일 경로).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UKernelAbilitySet> GrantedAbilitySet;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TArray<FKernelChanceStyleRule> ChanceStyleRules;

	bool GetChanceRange(EItemRarity Rarity, float& OutMin, float& OutMax) const
	{
		if (!bOverrideChanceRange) return false;   // 꺼져 있으면 전역으로 넘김
		return KernelAffixChance::FindRange(ChanceRangeOverride, Rarity, OutMin, OutMax);
	}
	
	/** 툴팁 문구 생성 헬퍼 */
	FText BuildDescription(float RolledChance) const
	{
		const int32 Percent = FMath::RoundToInt(RolledChance * 100.f);
		const FName Style = GetChanceStyleName(Percent);

		// 결과 예: <ChanceYellow>7%</>
		// %%는 Printf에서 리터럴 '%' 하나를 의미
		const FString Markup = FString::Printf(TEXT("<%s>%d%%</>"), *Style.ToString(), Percent);

		return FText::Format(DescriptionFormat, FText::FromString(Markup));
	}
	
private:
	FName GetChanceStyleName(int32 Percent) const
	{
		const FKernelChanceStyleRule* Best = nullptr;

		// 배열 순서와 무관하게, 조건을 만족하는 규칙 중 가장 높은 구간을 선택
		for (const FKernelChanceStyleRule& Rule : ChanceStyleRules)
		{
			if (Percent >= Rule.MinPercent)
			{
				if (!Best || Rule.MinPercent > Best->MinPercent)
				{
					Best = &Rule;
				}
			}
		}
		return Best ? Best->StyleName : FName("Default");   // 폴백도 Default로
	}
};