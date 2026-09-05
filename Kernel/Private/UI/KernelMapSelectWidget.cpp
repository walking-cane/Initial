// KernelLobbyWidget.cpp
#include "UI/KernelMapSelectWidget.h"

#include "CommonButtonBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Game/KernelGameState.h"
#include "Game/KernelGameModeBase.h"
#include "Game/KernelMapDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "Messages/KernelVerbMessage.h"
#include "UI/KernelMapSlotWidget.h"

void UKernelMapSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedGameState = GetWorld()->GetGameState<AKernelGameState>();
	if (!CachedGameState)
	{
		UE_LOG(LogTemp,Error,TEXT("[W_Lobby] GameState is null"));
		return;
	}
	
	// 맵 로스터 → 슬롯 동적 생성
	if (!MapListBox)
	{
		UE_LOG(LogTemp,Error,TEXT("[W_Lobby] Maplistbox is null"));
		return;
	}
	
	if (!MapSlotClass)
	{
		UE_LOG(LogTemp,Error,TEXT("[W_Lobby] Mapslotclass is null"));
		return;
	}
	
	if (MapListBox && MapSlotClass)
	{
		MapListBox->ClearChildren();
		MapSlots.Empty();
		
		UE_LOG(LogTemp,Log,TEXT("[W_Lobby] ClearChildren"));

		const auto& Roster = CachedGameState->GetMapRoster();
		
		for (int32 i = 0; i < Roster.Num(); ++i)
		{
			UKernelMapSlotWidget* MapSlot = CreateWidget<UKernelMapSlotWidget>(GetOwningPlayer(), MapSlotClass);
			if (!MapSlot) continue;
			MapSlot->InitSlot(i, Roster[i]);
			MapSlot->OnSlotClicked.BindUObject(this, &UKernelMapSelectWidget::OnMapSlotClicked);
			MapListBox->AddChild(MapSlot);
			MapSlots.Add(MapSlot);
			
			UE_LOG(LogTemp,Log,TEXT("[W_Lobby] MapSlot Added"));
		}
	}

	// START: 호스트 전용 + 맵 선택 전엔 비활성 (요구 1)
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UKernelMapSelectWidget::OnStartClicked);
		if (GetOwningPlayer() && !GetOwningPlayer()->HasAuthority())
		{
			StartButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	//if (MapImage) SetVisibility(ESlateVisibility::Collapsed);

	// GameState 이벤트 구독: 호스트 선택 변화, 핑
	CachedGameState->OnMapSelectionChanged.AddUObject(this, &UKernelMapSelectWidget::RefreshHostSelection);
	CachedGameState->OnMapPing.AddUObject(this, &UKernelMapSelectWidget::HandleMapPing);

	// 전원 PlayerState 투표 구독 (늦은 접속자 폴링 — 캐릭터 로비 때 검증된 패턴)
	GetWorld()->GetTimerManager().SetTimer(PlayerPollHandle, this,
		&UKernelMapSelectWidget::BindToPlayerStates, 0.5f, true, 0.f);
	
	FKernelVerbMessage Message;
	Message.Magnitude = -1;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_UI_ToggleLayout, Message);

	RefreshHostSelection();
}

void UKernelMapSelectWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlayerPollHandle);
	}
	for (TWeakObjectPtr<AKernelPlayerState>& WeakPS : BoundPlayerStates)
	{
		if (AKernelPlayerState* PS = WeakPS.Get())
		{
			PS->OnMapVoteChanged.RemoveAll(this);
		}
	}
	BoundPlayerStates.Empty();

	if (CachedGameState)
	{
		CachedGameState->OnMapSelectionChanged.RemoveAll(this);
		CachedGameState->OnMapPing.RemoveAll(this);
	}
	
	FKernelVerbMessage Message;
	Message.Magnitude = 1;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_UI_ToggleLayout, Message);
	
	Super::NativeDestruct();
}

void UKernelMapSelectWidget::BindToPlayerStates()
{
	if (!CachedGameState) return;

	bool bFoundNew = false;
	for (APlayerState* PS : CachedGameState->PlayerArray)
	{
		AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (KPS && !BoundPlayerStates.Contains(KPS))
		{
			KPS->OnMapVoteChanged.AddUObject(this, &UKernelMapSelectWidget::RefreshVotes);
			BoundPlayerStates.Add(KPS);
			bFoundNew = true;
		}
	}
	if (bFoundNew) RefreshVotes();
}

void UKernelMapSelectWidget::OnMapSlotClicked(int32 MapIndex)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	if (PC->HasAuthority())
	{
		// 요구 4: 선택 권한은 호스트만 — 호스트는 서버 로컬이니 GameState 직접 세팅
		CachedGameState->SetHostSelectedMap(MapIndex);
	}
	else
	{
		// 클라이언트: 투표 + 연타 핑 (요구 3·4) — 서버 RPC 하나로 둘 다 처리
		if (AKernelPlayerState* MyPS = GetOwningPlayerState<AKernelPlayerState>())
		{
			MyPS->ServerRPC_VoteMap(MapIndex);
		}
	}
}

void UKernelMapSelectWidget::OnStartClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC || !PC->HasAuthority()) return; // 호스트 전용 (버튼도 숨겨두지만 이중 방어)

	// 호스트는 서버 로컬이므로 GameMode 직접 접근 가능.
	// 위젯 교체는 여기서 하지 않는다 — 페이즈 복제 → 각 머신 HUD가 처리.
	if (AKernelGameModeBase* GM = GetWorld()->GetAuthGameMode<AKernelGameModeBase>())
	{
		GM->ConfirmMapAndBeginCharacterSelect();
	}
}

void UKernelMapSelectWidget::RefreshVotes()
{
	if (!CachedGameState) return;

	// 맵 인덱스별 투표자 이름 취합
	TMap<int32, FString> VotersByMap;
	for (APlayerState* PS : CachedGameState->PlayerArray)
	{
		const AKernelPlayerState* KPS = Cast<AKernelPlayerState>(PS);
		if (!KPS || KPS->GetVotedMapIndex() == INDEX_NONE) continue;

		FString& Names = VotersByMap.FindOrAdd(KPS->GetVotedMapIndex());
		if (!Names.IsEmpty()) Names += TEXT(", ");
		Names += KPS->GetPlayerName();
	}

	for (UKernelMapSlotWidget* MapSlot : MapSlots)
	{
		if (!MapSlot) continue;
		const FString* Names = VotersByMap.Find(MapSlot->GetMapIndex());
		MapSlot->SetVoterNames(Names ? *Names : FString());
	}
}

void UKernelMapSelectWidget::RefreshHostSelection()
{
	if (!CachedGameState) return;
	const int32 Selected = CachedGameState->GetHostSelectedMapIndex();
	const auto& Roster = CachedGameState->GetMapRoster();

	for (UKernelMapSlotWidget* MapSlot : MapSlots)
	{
		if (MapSlot->GetMapIndex() == Selected)
		{
			MapImage->SetBrushFromTexture(Roster[MapSlot->GetMapIndex()]->SelectImage.LoadSynchronous());
			MapImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	
	if (StartButton)
	{
		StartButton->SetIsEnabled(Selected != INDEX_NONE); // 요구 1
	}
}

void UKernelMapSelectWidget::HandleMapPing(const FString& PlayerName, int32 MapIndex)
{
	if (!CachedGameState || !PingChatText) return;

	const auto& Roster = CachedGameState->GetMapRoster();
	const FString MapName = Roster.IsValidIndex(MapIndex) && Roster[MapIndex]
		? Roster[MapIndex]->DisplayName.ToString() : TEXT("???");

	// 요구 3: "플레이어2님이 '사막'으로 가자고 합니다!"
	PingLines.Add(FString::Printf(TEXT("%s님이 '%s'(으)로 가자고 합니다!"), *PlayerName, *MapName));
	while (PingLines.Num() > 5) PingLines.RemoveAt(0); // 최근 5줄만

	PingChatText->SetText(FText::FromString(FString::Join(PingLines, TEXT("\n"))));
}

void UKernelMapSelectWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	SetFocus(); 

	// 2. CommonUI 세팅이 꼬일 때를 대비한 가장 확실한 마우스 커서 안전장치
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
		// PC->SetInputMode()는 쓰지 마세요! GetDesiredInputConfig가 해줍니다.
	}
}

void UKernelMapSelectWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
	}
}

TOptional<FUIInputConfig> UKernelMapSelectWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}
