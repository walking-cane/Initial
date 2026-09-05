#include "Item/KernelItemManager.h"

#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemFragment_Cosmetic.h"
#include "Item/KernelItemFragment_Equippable.h"
#include "Net/UnrealNetwork.h"

UKernelItemManager::UKernelItemManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

bool UKernelItemManager::AddItemToInventory(UKernelItemInstance* NewInstance)
{
	if (!CanAcceptItem(NewInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemManager] 슬롯 부족 — 획득 거부"));
		return false;
	}
	
	if (!NewInstance) return false;
	
	InventoryList.Add(NewInstance);
	AddReplicatedSubObject(NewInstance);

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (UKernelQuickBarComponent* QuickBar = Pawn->FindComponentByClass<UKernelQuickBarComponent>())
		{
			QuickBar->AddItemToFreeSlot(NewInstance);
		}
	}
	return true;
}

UKernelEquipmentInstance* UKernelItemManager::EquipItem(UKernelItemInstance* ItemInstance)
{
	UE_LOG(LogTemp,Warning,TEXT("[ItemManager] EquipItem"));
	if (!ItemInstance || GetOwner() == nullptr) return nullptr;

	const UKernelItemFragment_Equippable* EquipFrag = ItemInstance->FindFragmentByClass<UKernelItemFragment_Equippable>();
	const UKernelItemFragment_Cosmetic* CosmeticFrag = ItemInstance->FindFragmentByClass<UKernelItemFragment_Cosmetic>();

	if (EquipFrag && EquipFrag->EquipmentClass)
	{
		UKernelEquipmentInstance* NewEquip = NewObject<UKernelEquipmentInstance>(GetOwner(), EquipFrag->EquipmentClass);
		NewEquip->InstigatorItem = ItemInstance;
		
		EquipmentList.Add(NewEquip);
		AddReplicatedSubObject(NewEquip); // 등록을 OnEquipped 호출보다 먼저 해두는 게 안전합니다

		NewEquip->OnEquipped(GetOwner());
		UE_LOG(LogTemp, Warning, TEXT("[ItemManager] Equipment Added"));
		return NewEquip;
	}
	return nullptr;
}

void UKernelItemManager::UnEquipItem(UKernelItemInstance* ItemInstance)
{
	if (!ItemInstance) return;

	for (int32 i = EquipmentList.Num() - 1; i >= 0; --i)
	{
		UKernelEquipmentInstance* EquipInstance = EquipmentList[i];
		if (EquipInstance && EquipInstance->InstigatorItem == ItemInstance)
		{
			EquipInstance->UnEquipped(); // 장착 해제 로직 실행
			RemoveReplicatedSubObject(EquipInstance); // GC 전에 반드시 제거 (안 하면 크래시)
			EquipmentList.RemoveAt(i);
			break;
		}
	}
}

void UKernelItemManager::DropItem(UKernelItemInstance* DropInstance)
{
	if (!DropInstance) return;

	UnEquipItem(DropInstance);

	InventoryList.Remove(DropInstance);
	
	UE_LOG(LogTemp, Warning, TEXT("ItemManager :: Item Dropped. (Pickup Spawn Logic Here)"));
}

bool UKernelItemManager::CanAcceptItem(const UKernelItemInstance* Inst) const
{
	if (!Inst) return false;

	const bool bNeedsSlot = Inst->FindFragmentByClass<UKernelItemFragment_Equippable>() != nullptr;
	if (!bNeedsSlot) return true;

	APawn* Pawn = Cast<APawn>(GetOwner());
	UKernelQuickBarComponent* QuickBar = Pawn ? Pawn->FindComponentByClass<UKernelQuickBarComponent>() : nullptr;
	return QuickBar && QuickBar->HasFreeSlot();
}

void UKernelItemManager::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UKernelItemManager, InventoryList);
	DOREPLIFETIME(UKernelItemManager, EquipmentList);
}
