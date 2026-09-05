#pragma once

#include "CoreMinimal.h"
#include "KernelItemTypes.generated.h"

class UKernelAffixDefinition;
class UKernelItemDefinition;

/** 주의: 열거자 순서 = 등급 서열. 중간 삽입 금지 (MinRarity 비교가 순서에 의존) */
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Starting,
	Normal,
	Rare,
	Unique,
	Myth,
};

USTRUCT()
struct FKernelRarityColor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly) 
	EItemRarity Rarity = EItemRarity::Normal;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color = FLinearColor::White;
};

/** 무기 개체에 실제로 붙은 접사 (롤 결과) */
USTRUCT(BlueprintType)
struct FKernelAppliedAffix
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) 
	TObjectPtr<UKernelAffixDefinition> Affix = nullptr;
	
	UPROPERTY(BlueprintReadOnly) 
	float RolledChance = 0.f;
	
	/** 이 접사에 사용한 무료 리롤 횟수 */
	UPROPERTY(BlueprintReadOnly)
	uint8 RerollsUsed = 0;
	
	/** Roll할 접사가 Exclusive 인가 */
	UPROPERTY(BlueprintReadOnly)
	bool bFromExclusivePool = false;
};

/**
 * 드롭 1건의 확정된 결과.
 * 서버가 롤해서 클라에 보내고, 클라가 습득/공유 요청 시 서버 장부와 대조된다.
 */
USTRUCT(BlueprintType)
struct FKernelItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) TObjectPtr<UKernelItemDefinition> ItemDef = nullptr;
	UPROPERTY(BlueprintReadOnly) EItemRarity Rarity = EItemRarity::Normal;
	UPROPERTY(BlueprintReadOnly) float Level = 1.f;
	UPROPERTY(BlueprintReadOnly) TArray<FKernelAppliedAffix> Affixes;

	bool IsValid() const { return ItemDef != nullptr; }
};

USTRUCT()
struct FKernelRarityChanceRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Normal;

	/** 발동 확률 하한 (퍼센트 표기) */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float MinPercent = 5.f;

	/** 발동 확률 상한 (퍼센트 표기) */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float MaxPercent = 15.f;
};

namespace KernelAffixChance
{
	inline bool FindRange(const TArray<FKernelRarityChanceRange>& Ranges,
						  EItemRarity Rarity, float& OutMin, float& OutMax)
	{
		for (const FKernelRarityChanceRange& Entry : Ranges)
		{
			if (Entry.Rarity == Rarity)
			{
				OutMin = FMath::Min(Entry.MinPercent, Entry.MaxPercent) / 100.f;
				OutMax = FMath::Max(Entry.MinPercent, Entry.MaxPercent) / 100.f;
				return true;
			}
		}
		return false;
	}
}