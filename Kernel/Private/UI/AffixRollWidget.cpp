#include "UI/AffixRollWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelAffixRollLibrary.h"
#include "KernelCharacter/KernelPlayerController.h"
#include "Messages/KernelInteractionMessage.h"
#include "UI/AffixEntryWidget.h"
#include "UI/WeaponSlotEntryWidget.h"

void UAffixRollWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	RollAffixListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FGameplayTag::RequestGameplayTag("Interact.RollAffix"), this, &ThisClass::OnOpen);

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWindow);
	}

	if (AKernelPlayerController* PC = GetOwningPlayer<AKernelPlayerController>())
	{
		PC->OnAffixRerolled.AddUObject(this, &ThisClass::HandleAffixRerolled);
	}
}

void UAffixRollWidget::NativeDestruct()
{
	RollAffixListenerHandle.Unregister();

	if (AKernelPlayerController* PC = GetOwningPlayer<AKernelPlayerController>())
	{
		PC->OnAffixRerolled.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UAffixRollWidget::OnOpen(FGameplayTag Channel, const FKernelInteractionMessage& Message)
{
	// 지난번 선택이 남아 있으면 엉뚱한 무기로 요청이 나간다
	SelectedWeapon    = nullptr;
	SelectedSlotIndex = INDEX_NONE;

	if (AffixListBox) AffixListBox->ClearChildren();

	BuildWeaponList();

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

void UAffixRollWidget::CloseWindow()
{
	PlayAnimationReverse(BuildAnimation);
	/*
	SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
	*/
}

void UAffixRollWidget::BuildWeaponList()
{
	if (!WeaponListBox || !WeaponEntryClass) return;

	WeaponListBox->ClearChildren();

	APawn* Pawn = GetOwningPlayerPawn();
	UKernelQuickBarComponent* Qbr = Pawn ? Pawn->FindComponentByClass<UKernelQuickBarComponent>() : nullptr;

	if (!Qbr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AffixRoll] QuickBarComponent를 찾지 못했습니다"));
		return;
	}

	const TArray<TObjectPtr<UKernelItemInstance>>& Slots = Qbr->GetSlots();

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		UKernelItemInstance* Weapon = Slots[i];
		if (!Weapon || !Weapon->ItemDef) continue;   // 빈 슬롯

		UWeaponSlotEntryWidget* Entry =
			CreateWidget<UWeaponSlotEntryWidget>(GetOwningPlayer(), WeaponEntryClass);
		if (!Entry) continue;

		Entry->SetEntry(Weapon, i);
		Entry->OnWeaponSelected.BindUObject(this, &ThisClass::HandleWeaponSelected);

		WeaponListBox->AddChild(Entry);
	}
}

void UAffixRollWidget::HandleWeaponSelected(UKernelItemInstance* InWeapon, int32 InSlotIndex)
{
	SelectedWeapon    = InWeapon;
	SelectedSlotIndex = InSlotIndex;

	BuildAffixRows();
	
	if (BuildAnimation)
	{
		PlayAnimationForward(BuildAnimation);
	}
}

void UAffixRollWidget::BuildAffixRows()
{
	if (!AffixListBox || !AffixEntryClass) return;

	AffixListBox->ClearChildren();
	if (!SelectedWeapon.IsValid()) return;

	const TArray<FKernelAppliedAffix>& Affixes = SelectedWeapon->GetAffixes();

	for (int32 i = 0; i < Affixes.Num(); ++i)
	{
		if (!Affixes[i].Affix) continue;

		UAffixEntryWidget* Row = CreateWidget<UAffixEntryWidget>(GetOwningPlayer(), AffixEntryClass);
		if (!Row) continue;

		Row->SetEntry(UKernelAffixRollLibrary::MakeAffixDetail(Affixes[i]), i);
		Row->SetRerollAllowed(true);   // 리롤 버튼은 이 창에서만 켠다
		Row->OnRerollRequested.BindUObject(this, &ThisClass::HandleAffixRerollRequested);

		AffixListBox->AddChild(Row);
	}
}

void UAffixRollWidget::HandleAffixRerollRequested(int32 AffixIndex, FName AffixId)
{
	if (SelectedSlotIndex == INDEX_NONE) return;

	if (AKernelPlayerController* PC = GetOwningPlayer<AKernelPlayerController>())
	{
		PC->Server_RerollAffix(SelectedSlotIndex, AffixIndex, AffixId);
	}
}

void UAffixRollWidget::HandleAffixRerolled(
	int32 InSlotIndex, int32 AffixIndex, const FKernelAppliedAffix& NewApplied)
{
	if (InSlotIndex != SelectedSlotIndex) return;   // 다른 무기 결과면 무시
	if (!AffixListBox) return;

	for (int32 i = 0; i < AffixListBox->GetChildrenCount(); ++i)
	{
		UAffixEntryWidget* Row = Cast<UAffixEntryWidget>(AffixListBox->GetChildAt(i));
		if (Row && Row->GetAffixIndex() == AffixIndex)
		{
			Row->ApplyRerollResult(NewApplied.RolledChance, NewApplied.RerollsUsed);
			break;
		}
	}
}