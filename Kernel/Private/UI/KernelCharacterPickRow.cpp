// KernelPlayerPickRow.cpp
#include "UI/KernelCharacterPickRow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"

void UKernelCharacterPickRow::UpdateRow(const FText& PlayerName, const UKernelCharacterDefinition* Def, bool bIsMe)
{
	if (PlayerNameText) PlayerNameText->SetText(PlayerName);

	UTexture2D* Portrait = Def ? Def->Portrait.LoadSynchronous() : nullptr;

	if (PortraitImage)
	{
		if (Portrait)
		{
			PortraitImage->SetBrushFromTexture(Portrait);
			PortraitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PortraitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 아직 안 고른 플레이어는 빈 표시
	if (EmptyIcon)
	{
		EmptyIcon->SetVisibility(Portrait ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}

	if (MyHighlight)
	{
		MyHighlight->SetVisibility(bIsMe ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}