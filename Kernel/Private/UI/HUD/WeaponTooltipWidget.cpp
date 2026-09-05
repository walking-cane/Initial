#include "UI/HUD/WeaponTooltipWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Item/KernelItemBalanceSettings.h"
#include "Item/KernelItemDefinition.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelAffixRollLibrary.h"
#include "UI/AffixEntryWidget.h"

void UWeaponTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UWeaponTooltipWidget::ShowFor(UKernelItemInstance* InWeapon)
{
	if (!InWeapon || !InWeapon->ItemDef)
	{
		HideTooltip();
		return;
	}

	if (WeaponName)
	{
		WeaponName->SetText(InWeapon->ItemDef->DisplayName);
	}

	if (RarityBorder)
	{
		RarityBorder->SetBrushTintColor(
			GetDefault<UKernelItemBalanceSettings>()->GetRarityColor(InWeapon->Rarity));
	}

	if (AffixListBox && AffixEntryClass)
	{
		AffixListBox->ClearChildren();

		const TArray<FKernelAppliedAffix>& Affixes = InWeapon->GetAffixes();

		for (int32 i = 0; i < Affixes.Num(); ++i)
		{
			if (!Affixes[i].Affix) continue;

			UAffixEntryWidget* Row =
				CreateWidget<UAffixEntryWidget>(GetOwningPlayer(), AffixEntryClass);
			if (!Row) continue;

			Row->SetEntry(UKernelAffixRollLibrary::MakeAffixDetail(Affixes[i]), i);
			// SetRerollAllowed를 부르지 않음 → 리롤 버튼 자동 숨김
			AffixListBox->AddChild(Row);
		}
	}

	// Visible로 두면 툴팁이 커서를 가로채 호버가 끊긴다 (무한 깜빡임)
	SetVisibility(ESlateVisibility::HitTestInvisible);

	UpdatePosition();   // 첫 프레임에 (0,0)에서 튀는 것 방지
}

void UWeaponTooltipWidget::HideTooltip()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWeaponTooltipWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	UpdatePosition();
}

void UWeaponTooltipWidget::UpdatePosition()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(World);

	const float Scale = UWidgetLayoutLibrary::GetViewportScale(World);
	if (Scale <= 0.f) return;

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World) / Scale;

	FVector2D Size = GetCachedGeometry().GetLocalSize();
	if (Size.IsNearlyZero()) Size = GetDesiredSize();

	FVector2D Pos = MousePos + CursorOffset;

	// 화면 밖으로 나가면 커서 반대쪽으로 뒤집는다
	if (Pos.X + Size.X > ViewportSize.X) Pos.X = MousePos.X - Size.X - CursorOffset.X;
	if (Pos.Y + Size.Y > ViewportSize.Y) Pos.Y = MousePos.Y - Size.Y - CursorOffset.Y;

	SetPositionInViewport(Pos, false);
}