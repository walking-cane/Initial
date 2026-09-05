// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CommonUI/KernelMenuSelectButton.h"
#include "Components/TextBlock.h"


void UKernelMenuSelectButton::SetText()
{
	if (DesiredText)
	{
		DesiredText->SetText(ButtonText);
	}
}

void UKernelMenuSelectButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetText();
}

void UKernelMenuSelectButton::NativeOnHovered()
{
	Super::NativeOnHovered();
	
	UE_LOG(LogTemp,Log,TEXT("Hovered"));
	if (HoveredAnim)
	{
		PlayAnimationForward(HoveredAnim);
	}
}

void UKernelMenuSelectButton::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	
	if (HoveredAnim)
	{
		PlayAnimationReverse(HoveredAnim);
	}
}

void UKernelMenuSelectButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	
	UE_LOG(LogTemp,Log,TEXT("Clicked"));
	if (ClickedAnim)
	{
		PlayAnimationForward(ClickedAnim);
	}
}

