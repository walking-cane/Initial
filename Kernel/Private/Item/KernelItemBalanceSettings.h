#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "KernelItemTypes.h"
#include "KernelItemBalanceSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Kernel Item Balance"))
class KERNEL_API UKernelItemBalanceSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/** 등급별 접사 발동 확률 구간 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "Affix")
	TArray<FKernelRarityChanceRange> RarityChanceRanges;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "Affix", meta = (ClampMin = "0"))
	int32 FreeRerollsPerAffix = 1;

	virtual FName GetCategoryName() const override { return FName("Game"); }

	bool GetChanceRange(EItemRarity Rarity, float& OutMin, float& OutMax) const
	{
		return KernelAffixChance::FindRange(RarityChanceRanges, Rarity, OutMin, OutMax);
	}
	
	
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "Rarity")
	TArray<FKernelRarityColor> RarityColors;

	FLinearColor GetRarityColor(EItemRarity Rarity) const
	{
		for (const FKernelRarityColor& E : RarityColors)
		{
			if (E.Rarity == Rarity) return E.Color;
		}
		return FLinearColor::White;
	}
};
