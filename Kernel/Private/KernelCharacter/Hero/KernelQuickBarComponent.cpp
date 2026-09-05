// KernelQuickBarComponent.cpp 복구
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemManager.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Net/UnrealNetwork.h"

UKernelQuickBarComponent::UKernelQuickBarComponent()
{
	SetIsReplicatedByDefault(true);
	Slots.SetNum(NumSlots);
}

void UKernelQuickBarComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKernelQuickBarComponent, Slots);
	DOREPLIFETIME(UKernelQuickBarComponent, ActiveSlotIndex);
	DOREPLIFETIME(UKernelQuickBarComponent, EquippedItem);
}

void UKernelQuickBarComponent::BeginPlay()
{
	if (Slots.Num() != NumSlots)
	{
		Slots.SetNum(NumSlots);
	}

	Super::BeginPlay();
}

void UKernelQuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();
		
		OnRep_ActiveSlotIndex();
	}
}

void UKernelQuickBarComponent::EquipItemInSlot()
{
	if (!Slots.IsValidIndex(ActiveSlotIndex)) return;
	if (EquippedItem) return;
	
	if (UKernelItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (UKernelItemManager* ItemManager = FindItemManager())
		{
			EquippedItem = ItemManager->EquipItem(SlotItem);
			
			if (EquippedItem && GetOwner())
			{
				UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
				if (ASC)
				{
					FGameplayEventData Payload;
					Payload.OptionalObject = EquippedItem;
					Payload.Instigator = GetOwner();
					ASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Event.Weapon.Equip"), &Payload);
					
					UE_LOG(LogTemp, Error, TEXT("[QuickBar] Equip Trigger"))
				}
			}
		}
	}
}

void UKernelQuickBarComponent::UnequipItemInSlot()
{
	if (EquippedItem)
	{
		if (UKernelItemManager* ItemManager = FindItemManager())
		{
			ItemManager->UnEquipItem(EquippedItem->InstigatorItem);
		}
		EquippedItem = nullptr;
	}
}

bool UKernelQuickBarComponent::AddItemToFreeSlot(UKernelItemInstance* ItemInst)
{
	UE_LOG(LogTemp, Warning, TEXT("[QuickBar] Try to AddItem"));
	if (!ItemInst) return false;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i] == nullptr)
		{
			Slots[i] = ItemInst;
            
			OnRep_Slots();
            
			UE_LOG(LogTemp, Warning, TEXT("[QuickBar] Item Added to Slot %d"), i);

			
			if (ActiveSlotIndex == -1)
			{
				SetActiveSlotIndex(i);
			}
			
			return true;
		}
	}
    
	UE_LOG(LogTemp, Warning, TEXT("[QuickBar] No free slot available! (Num=%d)"), Slots.Num());
	return false;
}

bool UKernelQuickBarComponent::HasFreeSlot() const
{
	for (const TObjectPtr<UKernelItemInstance>& Slot : Slots)
	{
		if (Slot == nullptr) return true;
	}
	return false;
}

void UKernelQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FKernelQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;
	Message.ActiveItem = Slots[ActiveSlotIndex];

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_ActiveIndexChanged, Message);
}

void UKernelQuickBarComponent::OnRep_Slots()
{
	FKernelQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_SlotsChanged, Message);
}

UKernelItemManager* UKernelQuickBarComponent::FindItemManager() const
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		return Pawn->FindComponentByClass<UKernelItemManager>();
	}
	return nullptr;
}