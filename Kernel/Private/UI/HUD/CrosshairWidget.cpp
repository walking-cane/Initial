// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/CrosshairWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/KernelItemFragment_Weapon.h"
#include "Item/KernelItemInstance.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CrosshairListenerHandle = UGameplayMessageSubsystem::Get(this).
		RegisterListener(TAG_QuickBar_Message_ActiveIndexChanged, this, &ThisClass::OnActiveItemChanged);
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const float Alpha = FMath::Clamp(InDeltaTime * InterpSpeed, 0.f, 1.f);
	CurrentStyle = FKernelCrosshairStyle::Lerp(CurrentStyle, TargetStyle, Alpha);

	CurrentSpread = FMath::FInterpTo(CurrentSpread, 0.f, InDeltaTime, SpreadRecoverySpeed);

	ApplyStyle(CurrentStyle, CurrentSpread);
}

void UCrosshairWidget::ApplyStyle(const FKernelCrosshairStyle& Style, float ExtraGap)
{
	const float G = Style.Gap + ExtraGap;

	auto SetWing = [&](UImage* Img, const FVector2D& Dir, bool bVertical)
	{
		if (!Img) return;
		UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Img->Slot);
		if (!Slot) return;

		const FVector2D Size = bVertical
			? FVector2D(Style.WingThickness, Style.WingLength)
			: FVector2D(Style.WingLength, Style.WingThickness);

		Slot->SetSize(Size);
		// Alignment (0.5, 0.5) 기준이므로 중심에서 Gap + 절반만큼 밀어낸다
		Slot->SetPosition(Dir * (G + Style.WingLength * 0.5f));
		Img->SetColorAndOpacity(Style.Color);
	};

	SetWing(Top,    FVector2D(0, -1), true);
	SetWing(Bottom, FVector2D(0,  1), true);
	SetWing(Left,   FVector2D(-1, 0), false);
	SetWing(Right,  FVector2D( 1, 0), false);

	if (CenterDot)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CenterDot->Slot))
		{
			CanvasSlot->SetSize(FVector2D(Style.DotSize, Style.DotSize));
		}
		CenterDot->SetColorAndOpacity(Style.Color);
	}

	if (Ring)
	{
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Ring->Slot))
		{
			const float D = Style.RingRadius * 2.f;
			CanvasSlot->SetSize(FVector2D(D, D));
		}
		Ring->SetColorAndOpacity(Style.Color);
	}

	SetRenderTransformAngle(Style.Rotation);
}

void UCrosshairWidget::OnActiveItemChanged(FGameplayTag, const FKernelQuickBarActiveIndexChangedMessage& Message)
{
	if (Message.Owner != GetOwningPlayerPawn()) return;   // 남의 퀵바 메시지 무시

	if (Message.ActiveItem)
	{
		if (const UKernelItemFragment_Weapon* Frag = Message.ActiveItem->FindFragmentByClass<UKernelItemFragment_Weapon>())
		{
			SetTargetStyle(Frag->CrosshairStyle);
			return;
		}
	}
	
	SetTargetStyle(UnarmedStyle);   // 맨손 기본값 (EditDefaultsOnly로 하나 두세요)
}