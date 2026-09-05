#include "UI/HUD/KernelInventoryWidget.h"
#include "Messages/KernelVerbMessage.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Item/KernelItemInstance.h"
#include "UI/WeaponSlotEntryWidget.h"
#include "UI/HUD/WeaponTooltipWidget.h"

void UKernelInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::Collapsed);
	bIsOpen = false;
	
	InventoryListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		TAG_Input_Inventory, this, &ThisClass::ToggleInventory);
}

void UKernelInventoryWidget::NativeDestruct()
{
	if (Tooltip)
	{
		Tooltip->RemoveFromParent();
		Tooltip = nullptr;
	}

	InventoryListenerHandle.Unregister();
	Super::NativeDestruct();
}

void UKernelInventoryWidget::OpenInventory()
{
	bIsOpen = true;

	EnsureTooltip();
	RebuildWeaponTab();

	SetVisibility(ESlateVisibility::Visible);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
		PC->SetShowMouseCursor(true);
	}
}

void UKernelInventoryWidget::CloseInventory()
{
	bIsOpen = false;

	// 마우스가 칸 위에 올라간 채로 닫으면 Unhovered가 오지 않는다
	if (Tooltip) Tooltip->HideTooltip();

	SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void UKernelInventoryWidget::ToggleInventory(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	bIsOpen ? CloseInventory() : OpenInventory();
}

void UKernelInventoryWidget::EnsureTooltip()
{
	if (Tooltip || !TooltipClass) return;

	Tooltip = CreateWidget<UWeaponTooltipWidget>(GetOwningPlayer(), TooltipClass);
	if (Tooltip)
	{
		// 패널의 자식으로 넣으면 SetPositionInViewport가 무시되고 경계에서 잘린다
		Tooltip->AddToViewport(100);
		Tooltip->HideTooltip();
	}
}

void UKernelInventoryWidget::RebuildWeaponTab()
{
	if (!WeaponEntryBox || !WeaponEntryClass) return;

	WeaponEntryBox->ClearChildren();

	APawn* Pawn = GetOwningPlayerPawn();
	UKernelQuickBarComponent* Qbr =
		Pawn ? Pawn->FindComponentByClass<UKernelQuickBarComponent>() : nullptr;

	if (!Qbr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] QuickBarComponent를 찾지 못했습니다"));
		return;
	}

	const TArray<TObjectPtr<UKernelItemInstance>>& Slots = Qbr->GetSlots();

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		UWeaponSlotEntryWidget* Entry =
			CreateWidget<UWeaponSlotEntryWidget>(GetOwningPlayer(), WeaponEntryClass);
		if (!Entry) continue;

		// null이어도 넘긴다 — 빈 슬롯으로 그려진다
		Entry->SetEntry(Slots[i], i);

		Entry->OnWeaponSelected.BindUObject(this, &ThisClass::HandleWeaponSelected);
		Entry->OnWeaponHoverChanged.BindUObject(this, &ThisClass::HandleEntryHoverChanged);

		WeaponEntryBox->AddChild(Entry);
	}
}

void UKernelInventoryWidget::HandleWeaponSelected(UKernelItemInstance* Weapon, int32 SlotIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] 슬롯 %d 선택 (Weapon=%s)"),
		SlotIndex, *GetNameSafe(Weapon));

	// TODO: 장착 교체 / 버리기 등은 서버 RPC로
}

void UKernelInventoryWidget::HandleEntryHoverChanged(UKernelItemInstance* Weapon, bool bHovered)
{
	if (!Tooltip)
	{
		UE_LOG(LogTemp,Warning,TEXT("No Tooltip"))
		return;
	}
	
	bHovered ? Tooltip->ShowFor(Weapon) : Tooltip->HideTooltip();
}