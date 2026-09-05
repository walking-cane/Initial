// KernelItemRollLibrary.cpp
#include "Item/KernelAffixRollLibrary.h"

#include "Item/KernelItemBalanceSettings.h"
#include "Item/KernelItemDefinition.h"
#include "Item/KernelItemFragment_Weapon.h"
#include "Affixes/KernelAffixDefinition.h"

EItemRarity UKernelAffixRollLibrary::RollRarity(const TMap<EItemRarity, int32>& Weights)
{
	int32 Total = 0.f;
	for (const auto& Pair : Weights)
	{
		if (Pair.Value >= 0)
		{
		 	Total += FMath::Max(0, Pair.Value);
		}
	}
	if (Total <= 0.f) return EItemRarity::Normal;

	int32 Pick = FMath::RandRange(0, Total - 1);
	for (const auto& Pair : Weights)
	{
		Pick -= Pair.Value;
		if (Pick < 0.f) return Pair.Key;
	}
	return EItemRarity::Normal;
}

TArray<FKernelAppliedAffix> UKernelAffixRollLibrary::RollAffixes(const UKernelItemDefinition* ItemDef, EItemRarity Rarity)
{
	TArray<FKernelAppliedAffix> Result;
	if (!ItemDef) return Result;

	const UKernelItemFragment_Weapon* Frag =
		Cast<UKernelItemFragment_Weapon>(ItemDef->FindFragmentByClass(UKernelItemFragment_Weapon::StaticClass()));
	if (!Frag) return Result;   // 무기가 아니면 접사 없음
	
	auto MakeApplied = [Rarity](UKernelAffixDefinition* Def)
	{
		FKernelAppliedAffix A;
		A.Affix        = Def;
		A.RolledChance = RollAffixChance(Def, Rarity);
		return A;
	};
	
	auto PickWeighted = [](TArray<TObjectPtr<UKernelAffixDefinition>>& Pool) -> UKernelAffixDefinition*
	{
		float Total = 0.f;
		for (const TObjectPtr<UKernelAffixDefinition>& D : Pool)
		{
			Total += FMath::Max(0.01f, D->PickWeight);
		}
		if (Total <= 0.f) return nullptr;

		float Pick = FMath::FRand() * Total;
		for (int32 i = 0; i < Pool.Num(); ++i)
		{
			Pick -= FMath::Max(0.01f, Pool[i]->PickWeight);
			if (Pick <= 0.f)
			{
				UKernelAffixDefinition* Chosen = Pool[i];
				Pool.RemoveAtSwap(i);
				return Chosen;
			}
		}
		return nullptr;
	};

	// 무기별 제외 + 등급 미달 필터
	auto IsUnavailable = [Frag, Rarity](const TObjectPtr<UKernelAffixDefinition>& Def)
	{
		if (!Def) return true;
		if (Frag->ExcludedAffixes.Contains(Def)) return true;
		return static_cast<uint8>(Rarity) < static_cast<uint8>(Def->MinRarity);
	};

	// 배타 풀 — 정확히 1개
	{
		TArray<TObjectPtr<UKernelAffixDefinition>> Pool = Frag->ExclusiveAffixPool;
		Pool.RemoveAll(IsUnavailable);
		if (UKernelAffixDefinition* Chosen = PickWeighted(Pool))
		{
			Result.Add(MakeApplied(Chosen));
		}
	}

	// 일반 풀 — 등급이 정한 개수만큼
	{
		const FInt32Interval* Range = Frag->CommonAffixCountByRarity.Find(Rarity);
		if (!Range)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[Affix] %s — Rarity %d의 CommonAffixCountByRarity 미설정. 일반 접사 없음"),
				*GetNameSafe(ItemDef), (int32)Rarity);
			return Result;
		}

		const int32 Count = FMath::RandRange(
			FMath::Min(Range->Min, Range->Max), FMath::Max(Range->Min, Range->Max));

		TArray<TObjectPtr<UKernelAffixDefinition>> Pool = Frag->CommonAffixPool;
		Pool.RemoveAll(IsUnavailable);

		for (int32 i = 0; i < Count; ++i)
		{
			UKernelAffixDefinition* Chosen = PickWeighted(Pool);
			if (!Chosen) break;
			Result.Add(MakeApplied(Chosen));
		}
	}
	return Result;
}

float UKernelAffixRollLibrary::RollAffixChance(const UKernelAffixDefinition* Def, EItemRarity Rarity)
{
	if (!Def) return 0.f;

	float Min = 0.f, Max = 0.f;

	// 접사 자체 구간이 있으면 우선
	if (!Def->GetChanceRange(Rarity, Min, Max))
	{
		// 없으면 전역 등급 구간
		if (!GetDefault<UKernelItemBalanceSettings>()->GetChanceRange(Rarity, Min, Max))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Affix] %s — Rarity %d 확률 구간 미설정. 0으로 처리됨"),
				*GetNameSafe(Def), (int32)Rarity);
			return 0.f;
		}
	}

	return FMath::Clamp(FMath::FRandRange(Min, Max), 0.f, 1.f);
}

FKernelItemData UKernelAffixRollLibrary::RollItem(
	UKernelItemDefinition* ItemDef, const TMap<EItemRarity, int32>& RarityWeights, float Level)
{
	FKernelItemData Roll;
	if (!ItemDef) return Roll;

	Roll.ItemDef = ItemDef;
	Roll.Rarity  = RollRarity(RarityWeights);
	Roll.Level   = Level;
	Roll.Affixes = RollAffixes(ItemDef, Roll.Rarity);
	return Roll;
}

FKernelAffixDetail UKernelAffixRollLibrary::MakeAffixDetail(const FKernelAppliedAffix& Applied)
{
	FKernelAffixDetail Entry;
	if (!Applied.Affix) return Entry;

	Entry.AffixDef         = Applied.Affix;
	Entry.AffixName        = Applied.Affix->DisplayName;
	Entry.AffixDescription = Applied.Affix->BuildDescription(Applied.RolledChance);
	Entry.AffixIcon        = Applied.Affix->Icon.LoadSynchronous();
	Entry.RerollsUsed      = Applied.RerollsUsed;
	return Entry;
}
