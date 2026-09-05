// KernelItemRollLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Item/KernelItemTypes.h"
#include "Messages/KernelInteractionMessage.h"
#include "KernelAffixRollLibrary.generated.h"

UCLASS()
class KERNEL_API UKernelAffixRollLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 가중치 맵으로 등급 결정 */
	static EItemRarity RollRarity(const TMap<EItemRarity, int32>& Weights);

	/** ItemDef + 등급으로 접사를 뽑는다 (무기 프래그먼트가 없으면 빈 배열) */
	static TArray<FKernelAppliedAffix> RollAffixes(const UKernelItemDefinition* ItemDef, EItemRarity Rarity);
	static float RollAffixChance(const UKernelAffixDefinition* Def, EItemRarity Rarity);

	/** 드롭 1건 전체를 롤. 서버 전용 */
	static FKernelItemData RollItem(UKernelItemDefinition* ItemDef,
		const TMap<EItemRarity, int32>& RarityWeights, float Level = 1.f);
	
	static FKernelAffixDetail MakeAffixDetail(const FKernelAppliedAffix& Applied);
};