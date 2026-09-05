// KernelCharacterSlotWidget.cpp
#include "UI/CommonUI/KernelCharacterSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"

void UKernelCharacterSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UKernelCharacterSlot::HandleClicked);
		SlotButton->OnHovered.AddDynamic(this, &UKernelCharacterSlot::HandleHovered);
		SlotButton->OnUnhovered.AddDynamic(this, &UKernelCharacterSlot::HandleUnhovered);
	}
}

void UKernelCharacterSlot::InitSlot(int32 InIndex, const UKernelCharacterDefinition* Def, bool bUnlocked)
{
	CharacterIndex = InIndex;
	bIsUnlocked = bUnlocked;
	if (!Def) return;

	//if (NameText) NameText->SetText(Def->DisplayName);

	if (PortraitImage)
	{
		if (UTexture2D* Tex = Def->Portrait.LoadSynchronous()) // Definition에 Portrait 필드 추가 필요
		{
			PortraitImage->SetBrushFromTexture(Tex);
		}
		// 잠긴 캐릭터는 어둡게 (맵 슬롯과 동일한 처리 — 진짜 흑백은 Desaturate 머티리얼로)
		PortraitImage->SetColorAndOpacity(bIsUnlocked
			? FLinearColor::White
			: FLinearColor(0.25f, 0.25f, 0.25f, 1.f));
	}

	if (LockIcon)
	{
		LockIcon->SetVisibility(bIsUnlocked ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
	if (SelectedBorder) SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);

	// 잠긴 슬롯은 버튼 자체를 비활성 — 클릭 시도조차 막는다 (서버 검증은 그래도 유지)
	//if (SlotButton) SlotButton->SetIsEnabled(bIsUnlocked);
}

void UKernelCharacterSlot::HandleClicked()
{
	if (!bIsUnlocked) return;
	
	OnSlotClicked.ExecuteIfBound(CharacterIndex);
}

void UKernelCharacterSlot::HandleHovered()
{
	if (HoveredAnim)
	{
		PlayAnimationForward(HoveredAnim);
	}
}

void UKernelCharacterSlot::HandleUnhovered()
{
	if (HoveredAnim)
	{
		PlayAnimationReverse(HoveredAnim);
	}
}

void UKernelCharacterSlot::SetMyPick(bool bMine)
{
	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(bMine ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}
