#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Equippable.h"
// 주의: 아래 경로를 프로젝트의 실제 KernelAbilitySet 헤더 경로에 맞게 수정해 주세요.
#include "GameplayAbility/AbilitySets/KernelAbilitySet.h" 
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "Cosmetics/KernelCosmeticComponent.h"
#include "Net/UnrealNetwork.h"

void UKernelEquipmentInstance::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKernelEquipmentInstance, InstigatorItem);
}

void UKernelEquipmentInstance::OnEquipped(AActor* Owner)
{
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	if (Owner->HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		{
			if (InstigatorItem)
			{
				const UKernelItemFragment_Equippable* EquipFrag = InstigatorItem->FindFragmentByClass<UKernelItemFragment_Equippable>();
				
				if (EquipFrag && EquipFrag->GrantAbilitySet)
				{
					EquipFrag->GrantAbilitySet->GiveToAbilitySystem(
						ASC, 
						&GrantedAbilityHandles, 
						this);
					
					UE_LOG(LogTemp, Warning, TEXT("[Equip] GiveAbilitySet to %s (Item=%s)"),
						*GetNameSafe(ASC->GetOwnerActor()), *GetNameSafe(InstigatorItem));
				}
			}
		}
	}
}

void UKernelEquipmentInstance::UnEquipped()
{
	// ItemManager가 이 객체의 Outer일 수 있으므로 최상단 Owner를 찾아야함
	AActor* OwnerActor = GetTypedOuter<AActor>();

	if (OwnerActor && OwnerActor->HasAuthority())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
		{
			for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
			{
				if (Handle.IsValid())
				{
					ASC->ClearAbility(Handle);
				}
			}
		}
		GrantedAbilityHandles.Empty();
	}

	if (OwnerActor)
	{
		if (UKernelCosmeticComponent* Cosmetic = OwnerActor->FindComponentByClass<UKernelCosmeticComponent>())
		{
			Cosmetic->ChangeWeapon(nullptr);
		}
	}
	
	SpawnedEquippedActor = nullptr;
}