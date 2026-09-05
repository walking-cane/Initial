// KernelMapSlotWidget.cpp
#include "UI/KernelMapSlotWidget.h"
#include "CommonButtonBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Game/KernelMapDefinition.h"

void UKernelMapSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UKernelMapSlotWidget::HandleClicked);
		SlotButton->OnHovered.AddDynamic(this, &UKernelMapSlotWidget::HandleHovered);
		SlotButton->OnUnhovered.AddDynamic(this, &UKernelMapSlotWidget::HandleUnhovered);
	}
}

void UKernelMapSlotWidget::InitSlot(int32 InMapIndex, const UKernelMapDefinition* MapDef)
{
	MapIndex = InMapIndex;
	if (!MapDef) return;

	bLocked = MapDef->bLocked;

	if (MapNameText) MapNameText->SetText(MapDef->DisplayName);
	if (ThumbnailImage)
	{
		if (UTexture2D* Tex = MapDef->PreviewImage.LoadSynchronous())
		{
			ThumbnailImage->SetBrushFromTexture(Tex);
		}
		// 요구 2: 잠긴 맵은 흑백(어두운 회색 틴트) — 진짜 desaturation이 필요하면 머티리얼로 교체
		ThumbnailImage->SetColorAndOpacity(bLocked
			? FLinearColor(0.25f, 0.25f, 0.25f, 1.f)
			: FLinearColor::White);
	}
	if (LockIcon) LockIcon->SetVisibility(bLocked ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (VoterListText) VoterListText->SetText(FText::GetEmpty());
}

void UKernelMapSlotWidget::HandleClicked()
{
	if (bLocked) return; // 잠긴 맵은 클릭 무시 (서버도 이중 검증하지만 UI에서 1차 차단)
	PlayAnimation(ClickedAnim);
	OnSlotClicked.ExecuteIfBound(MapIndex);
}

void UKernelMapSlotWidget::HandleHovered()
{
	PlayAnimation(HoveredAnim);
}

void UKernelMapSlotWidget::HandleUnhovered()
{
	PlayAnimationReverse(HoveredAnim);
}

void UKernelMapSlotWidget::SetVoterNames(const FString& Names)
{
	if (VoterListText) VoterListText->SetText(FText::FromString(Names));
}