#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "KernelItemDefinition.h"
#include "Item/KernelItemTypes.h"
#include "KernelItemInstance.generated.h"

class UKernelAffixDefinition;

DECLARE_MULTICAST_DELEGATE(FOnAffixesChanged);

UCLASS(BlueprintType, Blueprintable)
class KERNEL_API UKernelItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	TArray<FKernelAppliedAffix>& GetAffixes() { return Affixes; };
	
	void InitFromRollResult(const FKernelItemData& Roll)
	{
		ItemDef = Roll.ItemDef;
		Rarity  = Roll.Rarity;
		Level   = Roll.Level;
		Affixes = Roll.Affixes;
	}
	
	const TArray<FKernelAppliedAffix>& GetAffixes() const { return Affixes; }
	
	UPROPERTY(Replicated)
	TObjectPtr<const UKernelItemDefinition> ItemDef;
	
	UPROPERTY(Replicated)
	EItemRarity Rarity;
	
	UPROPERTY(Replicated)
	float Level = 1.f;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		if (ItemDef != nullptr)
		{
			return Cast<ResultClass>(ItemDef->FindFragmentByClass(ResultClass::StaticClass()));
		}
		return nullptr;
	}
	
	bool TryRerollAffix(int32 AffixIndex, FName ExpectedAffixId, FKernelAppliedAffix& OutApplied);
	
	FOnAffixesChanged OnAffixesChanged;
	
protected:
	UPROPERTY(Replicated)
	TArray<FKernelAppliedAffix> Affixes;
};