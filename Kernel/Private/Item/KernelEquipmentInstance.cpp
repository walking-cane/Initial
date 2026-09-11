#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Equippable.h"
// 주의: 아래 경로를 프로젝트의 실제 KernelAbilitySet 헤더 경로에 맞게 수정해 주세요.
#include "GameplayAbility/AbilitySets/KernelAbilitySet.h" 
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "Cosmetics/KernelCosmeticComponent.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

void UKernelEquipmentInstance::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKernelEquipmentInstance, InstigatorItem);
}

void UKernelEquipmentInstance::OnEquipped(AActor* Owner)
{
	if (!Owner || !Owner->HasAuthority()) return;
	if (!InstigatorItem) return;

	// [변경] AbilitySet이 UKernelAbilitySystemComponent*를 요구하므로 캐스팅
	UKernelAbilitySystemComponent* KernelASC = Cast<UKernelAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner));
	if (!KernelASC) return;

	const UKernelItemFragment_Equippable* EquipFrag =
		InstigatorItem->FindFragmentByClass<UKernelItemFragment_Equippable>();
	if (!EquipFrag) return;

	if (EquipFrag->GrantAbilitySet)
	{
		// 어빌리티 + GE가 여기서 한 번에 부여되고, 핸들도 GrantedHandles에 함께 쌓임
		EquipFrag->GrantAbilitySet->GiveToAbilitySystem(KernelASC, &GrantedHandles, this);

		UE_LOG(LogTemp, Warning, TEXT("[Equip] GiveAbilitySet to %s (Item=%s)"),
			*GetNameSafe(KernelASC->GetOwnerActor()), *GetNameSafe(InstigatorItem));
	}
}

void UKernelEquipmentInstance::UnEquipped()
{
	AActor* OwnerActor = GetTypedOuter<AActor>();

	if (OwnerActor && OwnerActor->HasAuthority())
	{
		if (UKernelAbilitySystemComponent* KernelASC = Cast<UKernelAbilitySystemComponent>(
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor)))
		{
			// [변경] ClearAbility 수동 루프 → 어빌리티/GE/어트리뷰트셋 한 번에 반납
			GrantedHandles.TakeFromAbilitySystem(KernelASC);
		}
	}

	if (OwnerActor)
	{
		if (UKernelCosmeticComponent* Cosmetic = OwnerActor->FindComponentByClass<UKernelCosmeticComponent>())
		{
			Cosmetic->ClearWeaponAttachEntries();
		}
	}

	SpawnedEquippedActor = nullptr;
}