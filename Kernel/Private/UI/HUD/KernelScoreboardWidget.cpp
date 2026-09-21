// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelScoreBoardWidget.h"

#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "Messages/KernelVerbMessage.h"
#include "UI/KernelScoreboardSlotEntry.h"


void UKernelScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 엔트리를 최대 인원만큼 미리 생성. 이후 재생성 없음
	for (int32 i = 0; i < MaxPlayers; ++i)
	{
		UKernelScoreboardSlotEntry* Entry =
			CreateWidget<UKernelScoreboardSlotEntry>(GetOwningPlayer(), EntryClass);
		if (!Entry) continue;

		Entry->SetVisibility(ESlateVisibility::Collapsed);
		EntryPool.Add(Entry);
		BoardEntry->AddChild(Entry);
	}

	UGameplayMessageSubsystem& Msg = UGameplayMessageSubsystem::Get(this);
	ToggleHandle  = Msg.RegisterListener(TAG_Input_ScoreBoard, this, &ThisClass::OnToggle);
	RefreshHandle = Msg.RegisterListener(TAG_Scoreboard_Refresh, this, &ThisClass::RefreshEntries);

	SetVisibility(ESlateVisibility::Collapsed); 
	
	RefreshEntries(TAG_Scoreboard_Refresh, FKernelVerbMessage());
}

void UKernelScoreboardWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKernelScoreboardWidget::RefreshEntries(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS) return;

	const int32 Count = FMath::Min(GS->PlayerArray.Num(), EntryPool.Num());

	for (int32 i = 0; i < EntryPool.Num(); ++i)
	{
		if (i < Count)
		{
			EntryPool[i]->SetEntry(Cast<AKernelPlayerState>(GS->PlayerArray[i]));
			EntryPool[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			EntryPool[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UKernelScoreboardWidget::OnToggle(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
	const bool bShow = (GetVisibility() == ESlateVisibility::Collapsed);
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		bShow ? 
		PC->SetInputMode( FInputModeGameAndUI()) :
		PC->SetInputMode( FInputModeGameOnly());
		
		bShow ?
		PC->SetShowMouseCursor(true) :
		PC->SetShowMouseCursor(false);
	}
	
	UE_LOG(LogTemp,Log,TEXT("[Scoreboard] Toggle Visibility : %d"), bShow);
}