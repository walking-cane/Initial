#include "UI/WeaponSlotEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Item/KernelItemBalanceSettings.h"
#include "Item/KernelItemDefinition.h"
#include "Item/KernelItemInstance.h"

void UWeaponSlotEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked  .AddDynamic(this, &ThisClass::HandleClicked);
		SelectButton->OnHovered  .AddDynamic(this, &ThisClass::HandleHovered);
		SelectButton->OnUnhovered.AddDynamic(this, &ThisClass::HandleUnhovered);
	}
}

void UWeaponSlotEntryWidget::SetEntry(UKernelItemInstance* InWeapon, int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	Weapon = (InWeapon && InWeapon->ItemDef) ? InWeapon : nullptr;

	// ── 빈 슬롯 ──
	if (!Weapon)
	{
		if (WeaponImage) WeaponImage->SetVisibility(ESlateVisibility::Hidden);
		if (Border)      Border->SetBrushTintColor(EmptySlotColor);
		return;
	}

	// ── 무기 있음 ──
	if (WeaponImage)
	{
		WeaponImage->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (UTexture2D* Icon = Weapon->ItemDef->Icon.LoadSynchronous())
		{
			WeaponImage->SetBrushFromTexture(Icon);
		}
	}

	if (Border)
	{
		Border->SetBrushTintColor(
			GetDefault<UKernelItemBalanceSettings>()->GetRarityColor(Weapon->Rarity));
	}
}

void UWeaponSlotEntryWidget::HandleClicked()
{
	if (SlotIndex == INDEX_NONE) return;

	OnWeaponSelected.ExecuteIfBound(Weapon, SlotIndex);
}

void UWeaponSlotEntryWidget::HandleHovered()
{
	OnWeaponHoverChanged.ExecuteIfBound(Weapon, Weapon != nullptr);
}

void UWeaponSlotEntryWidget::HandleUnhovered()
{
	OnWeaponHoverChanged.ExecuteIfBound(nullptr, false);
}