// CharacterSelectWidget.cpp
#include "UI/CharacterSelectWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Game/KernelGameModeBase.h"
#include "Game/KernelGameState.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"
#include "UI/KernelCharacterPickRow.h"
#include "UI/CommonUI/KernelCharacterSlot.h"

void UCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// [버그 수정] 기존 코드는 이 대입이 없어 항상 null → 즉시 return이었다
	CachedGameState = GetWorld()->GetGameState<AKernelGameState>();
	if (!CachedGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("[W_CharSelect] GameState is null"));
		return;
	}

	BuildSlots();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnStartButtonClicked);
		// START는 호스트 전용 (맵 위젯과 동일 정책)
		if (GetOwningPlayer() && !GetOwningPlayer()->HasAuthority())
		{
			StartButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	if (PickedImage)
	{
		PickedImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0));
	}

	// 전원의 픽 변화 + 내 해금 목록 도착을 구독 (0.5초 폴링 — 맵 위젯과 동일 패턴)
	GetWorld()->GetTimerManager().SetTimer(PlayerPollHandle, this,
		&UCharacterSelectWidget::BindToPlayerStates, 0.5f, true, 0.f);

	RefreshCharacterPicks();
	RefreshStartButton();
}

void UCharacterSelectWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlayerPollHandle);
	}
	for (TWeakObjectPtr<AKernelPlayerState>& WeakPS : BoundPlayerStates)
	{
		if (AKernelPlayerState* PS = WeakPS.Get())
		{
			PS->OnLobbySelectionChanged.RemoveAll(this);
			PS->OnUnlocksChanged.RemoveAll(this);
		}
	}
	BoundPlayerStates.Empty();

	Super::NativeDestruct();
}

void UCharacterSelectWidget::BuildSlots()
{
	if (!CachedGameState || !CharacterListBox || !CharacterSlotClass) return;

	// ★ 잠금 판정의 출처: GameState가 아니라 '내' PlayerState (개인 해금이므로)
	AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>();

	CharacterListBox->ClearChildren();
	CharacterSlots.Empty();

	const auto& Roster = CachedGameState->GetCharacterRoster();
	for (int32 i = 0; i < Roster.Num(); ++i)
	{
		UKernelCharacterSlot* CharacterSlot = CreateWidget<UKernelCharacterSlot>(GetOwningPlayer(), CharacterSlotClass);
		if (!CharacterSlot) continue;

		const bool bUnlocked = (MyPS && Roster[i]) ? MyPS->IsCharacterUnlocked(Roster[i]->CharacterId) : false;
		CharacterSlot->InitSlot(i, Roster[i], bUnlocked);
		CharacterSlot->OnSlotClicked.BindUObject(this, &UCharacterSelectWidget::RequestCharacter);

		CharacterListBox->AddChild(CharacterSlot);
		CharacterSlots.Add(CharacterSlot);
	}

	RefreshCharacterPicks(); // 재생성 후 픽 표시 복원
}

void UCharacterSelectWidget::BindToPlayerStates()
{
	if (!CachedGameState) return;

	AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>();
	bool bFoundNew = false;

	for (APlayerState* PS : CachedGameState->PlayerArray)
	{
		AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (!KPS || BoundPlayerStates.Contains(KPS)) continue;

		KPS->OnLobbySelectionChanged.AddUObject(this, &UCharacterSelectWidget::RefreshCharacterPicks);

		// 내 해금 목록이 복제 도착하면 슬롯을 다시 짓는다
		// (RPC 신고 → 복제가 위젯 생성보다 늦을 수 있어 전부 잠긴 채 그려지는 것 방지)
		if (KPS == MyPS)
		{
			KPS->OnUnlocksChanged.AddUObject(this, &UCharacterSelectWidget::BuildSlots);
		}

		BoundPlayerStates.Add(KPS);
		bFoundNew = true;
	}

	if (bFoundNew)
	{
		RefreshCharacterPicks();
		RefreshStartButton();
	}
}

void UCharacterSelectWidget::RequestCharacter(int32 Index)
{
	AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>();
	if (MyPS)
	{
		MyPS->ServerRPC_ChangeCharacter(Index); // 서버가 페이즈·범위·해금 검증
	}
	
	if (PickedCharNameAnim && PickedCharName)
	{
		PickedCharName->SetText(MyPS->GetCharacterDefinition()->DisplayName);
	}
	
	if (GetOwningPlayerPawn()->IsLocallyControlled())
	{
		PlayAnimationForward(PickedCharNameAnim);
	}
}

void UCharacterSelectWidget::RefreshCharacterPicks()
{
	if (!CachedGameState) return;
	
	const AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>();
	const int32 MyPick = MyPS ? MyPS->GetSelectedCharacterIndex() : INDEX_NONE;

	for (UKernelCharacterSlot* CharacterSlot : CharacterSlots)
	{
		if (!CharacterSlot) continue;
		CharacterSlot->SetMyPick(CharacterSlot->GetCharacterIndex() == MyPick);
	}

	RefreshStartButton(); // 픽이 바뀔 때마다 START 활성 조건 재평가
	RefreshPlayerList(); // 픽이 바뀔 때마다 플레이어 픽 상태 변경.
}

void UCharacterSelectWidget::RefreshStartButton()
{
	if (!StartButton || !CachedGameState) return;

	// 전원이 캐릭터를 골라야 출발 가능
	bool bAllPicked = CachedGameState->PlayerArray.Num() > 0;
	for (APlayerState* PS : CachedGameState->PlayerArray)
	{
		const AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (!KPS || KPS->GetSelectedCharacterIndex() == INDEX_NONE)
		{
			bAllPicked = false;
			break;
		}
	}
	StartButton->SetIsEnabled(bAllPicked);
}

void UCharacterSelectWidget::OnStartButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC || !PC->HasAuthority()) return;

	// [기존 버그] 맵 로스터로 StartGameWithMap을 부르던 코드는 제거.
	// 맵은 이미 GameState에 확정돼 있고, 여기선 출발 단계로 넘기기만 한다.
	if (AKernelGameModeBase* GM = GetWorld()->GetAuthGameMode<AKernelGameModeBase>())
	{
		GM->BeginDeparture();
	}
}

void UCharacterSelectWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	SetFocus();
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
	}
}

void UCharacterSelectWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
	}
}

TOptional<FUIInputConfig> UCharacterSelectWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void UCharacterSelectWidget::RefreshPlayerList()
{
	if (!CachedGameState || !PlayerListBox || !PlayerRowClass) return;

	const TArray<TObjectPtr<APlayerState>>& Players = CachedGameState->PlayerArray;
	const AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>();
	const auto& Roster = CachedGameState->GetCharacterRoster();

	// 인원수가 바뀔 때만 줄을 다시 짓는다 (매번 재생성하면 깜빡인다)
	if (PlayerRows.Num() != Players.Num())
	{
		PlayerListBox->ClearChildren();
		PlayerRows.Empty();

		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (UKernelCharacterPickRow* Row = CreateWidget<UKernelCharacterPickRow>(GetOwningPlayer(), PlayerRowClass))
			{
				PlayerListBox->AddChild(Row);
				PlayerRows.Add(Row);
			}
		}
	}

	// 내용 갱신
	for (int32 i = 0; i < PlayerRows.Num() && i < Players.Num(); ++i)
	{
		const AKernelPlayerState* KPS = Cast<AKernelPlayerState>(Players[i]);
		if (!KPS || !PlayerRows[i]) continue;

		const int32 Idx = KPS->GetSelectedCharacterIndex();
		const UKernelCharacterDefinition* Def = Roster.IsValidIndex(Idx) ? Roster[Idx] : nullptr;

		PlayerRows[i]->UpdateRow(FText::FromString(KPS->GetPlayerName()), Def, KPS == MyPS);
	}
}