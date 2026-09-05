#include "Item/KernelItemInstance.h"
#include "KernelItemBalanceSettings.h"
#include "Affixes/KernelAffixDefinition.h"
#include "Item/KernelAffixRollLibrary.h"
#include "Net/UnrealNetwork.h"

void UKernelItemInstance::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKernelItemInstance, ItemDef);
	DOREPLIFETIME(UKernelItemInstance, Rarity);
	DOREPLIFETIME(UKernelItemInstance, Affixes);
	DOREPLIFETIME(UKernelItemInstance, Level);
}

bool UKernelItemInstance::TryRerollAffix(
	int32 AffixIndex, FName ExpectedAffixId, FKernelAppliedAffix& OutApplied)
{
	AActor* Owner = GetTypedOuter<AActor>();
	if (!Owner || !Owner->HasAuthority()) return false;
	if (!Affixes.IsValidIndex(AffixIndex)) return false;

	FKernelAppliedAffix& Applied = Affixes[AffixIndex];
	if (!Applied.Affix) return false;
	if (Applied.Affix->AffixId != ExpectedAffixId) return false;

	const int32 MaxFree = GetDefault<UKernelItemBalanceSettings>()->FreeRerollsPerAffix;
	if (Applied.RerollsUsed >= MaxFree) return false;

	Applied.RolledChance = UKernelAffixRollLibrary::RollAffixChance(Applied.Affix, Rarity);
	Applied.RerollsUsed++;

	OutApplied = Applied;
	return true;
}