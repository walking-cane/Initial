// Fill out your copyright notice in the Description page of Project Settings.

#include "KernelCharacter/KernelPlayerController.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Game/KernelGameState.h"
#include "Game/KernelSaveGame.h"
#include "Interaction/KernelInteractionComponent.h"
#include "Affixes/KernelAffixDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemBalanceSettings.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemManager.h"
#include "Item/KernelItemPickup.h"
#include "Item/KernelAffixRollLibrary.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Messages/KernelVerbMessage.h"
#include "Subsystem/KernelSaveSubsystem.h"

void AKernelPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
	
	TryBindToGameState();
}

void AKernelPlayerController::TryBindToGameState()
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AKernelPlayerController::TryBindToGameState);
		return;
	}

	GS->OnGamePhaseChanged.AddUObject(this, &AKernelPlayerController::HandleGamePhase);
	HandleGamePhase(GS->GetGamePhase());
}

void AKernelPlayerController::ReportUnlocksToServer()
{
	AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>();
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();

	if (!PS || !GS)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AKernelPlayerController::ReportUnlocksToServer);
		return;
	}

	if (UKernelSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UKernelSaveSubsystem>())
	{
		PS->ServerRPC_ReportUnlockedCharacters(Save->GetUnlockedCharacterIds());
	}
}

UAbilitySystemComponent* AKernelPlayerController::GetAbilitySystemComponent() const
{
	if (AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
    
	return nullptr;
}

void AKernelPlayerController::Save_Unlock(FName CharacterId)
{
	if (UKernelSaveSubsystem* S = GetGameInstance()->GetSubsystem<UKernelSaveSubsystem>())
	{
		S->UnlockCharacter(CharacterId);
		ReportUnlocksToServer();
		UE_LOG(LogTemp, Warning, TEXT("[Save] Unlocked: %s"), *CharacterId.ToString());
	}
}

void AKernelPlayerController::Save_Lock(FName CharacterId)
{
	if (UKernelSaveSubsystem* S = GetGameInstance()->GetSubsystem<UKernelSaveSubsystem>())
	{
		S->GetSave()->UnlockedCharacterIds.Remove(CharacterId);
		S->SaveNow();
		ReportUnlocksToServer();
	}
}

void AKernelPlayerController::Save_ResetProfile()
{
	UGameplayStatics::DeleteGameInSlot(TEXT("KernelProfile"), 0);
	UE_LOG(LogTemp, Warning, TEXT("[Save] 프로필 삭제 — 재시작 시 기본값으로 생성됨"));
}

void AKernelPlayerController::Save_Dump()
{
	if (UKernelSaveSubsystem* S = GetGameInstance()->GetSubsystem<UKernelSaveSubsystem>())
	{
		for (const FName& Id : S->GetUnlockedCharacterIds())
			UE_LOG(LogTemp, Warning, TEXT("[Save/Local] %s"), *Id.ToString());
	}
	if (AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>())
	{
		// 서버가 인정한 목록과 로컬이 일치하는지 대조 — 신고 RPC 검증의 핵심
		UE_LOG(LogTemp, Warning, TEXT("[Save/PS] 서버 사본 확인 (PS: %s)"), *PS->GetPlayerName());
	}
}


int32 AKernelPlayerController::GrantLocalDrop(const FKernelItemData& Roll, const FTransform& Xform)
{
	if (!HasAuthority() || !Roll.IsValid()) return INDEX_NONE;

	const int32 Id = NextDropId++;
	PendingDrops.Add(Id, Roll);          // 장부 기록 — 위조·중복 사용 차단의 근거
	Client_SpawnLocalDrop(Id, Roll, Xform);
	
	return Id;
}

void AKernelPlayerController::Client_SpawnLocalDrop_Implementation(
	int32 InDropId, const FKernelItemData& InRoll, const FTransform& Xform)
{
	if (!PickupClass || !InRoll.IsValid()) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;
	Params.bDeferConstruction = true;

	AKernelItemPickup* P = GetWorld()->SpawnActorDeferred<AKernelItemPickup>(PickupClass, Xform, this, 
		nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!P) return;

	P->SetReplicates(false);   // 이 머신에만 존재
	P->bLocalOnly = true;
	P->DropId = InDropId;
	P->ApplyRoll(InRoll);
	
	P->FinishSpawning(Xform);
	//UE_LOG(LogTemp,Warning,TEXT("[PC] SpawnLocalDrop"))
}

bool AKernelPlayerController::ConsumeDrop(int32 InDropId, FKernelItemData& OutRoll)
{
	// 장부에 없거나 이미 소비된 ID면 거부 — 습득+공유 중복 사용도 여기서 막힌다
	if (const FKernelItemData* Found = PendingDrops.Find(InDropId))
	{
		OutRoll = *Found;
		PendingDrops.Remove(InDropId);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("[Drop] 유효하지 않은 DropId=%d (%s)"), InDropId, *GetName());
	return false;
}

void AKernelPlayerController::HandleGamePhase(EKernelGamePhase Phase)
{
	if (!IsLocalController()) return;
	
	if (Phase == EKernelGamePhase::Departing)
	{
		PlayerCameraManager->StartCameraFade(
		0.f, 
		1.f, 
		1.f, 
		FLinearColor::Black,
		/*bFadeAudio*/ true, 
		/*bHoldWhenFinished*/ true);
		
		FInputModeGameOnly GameMode;
		SetInputMode(GameMode);
	}
}

void AKernelPlayerController::Server_TakeDrop_Implementation(int32 InDropId)
{
	FKernelItemData TakenRoll;
	if (!ConsumeDrop(InDropId, TakenRoll)) return;

	APawn* P = GetPawn();
	UKernelItemManager* Mgr = P ? P->FindComponentByClass<UKernelItemManager>() : nullptr;
	if (!Mgr)
	{
		PendingDrops.Add(InDropId, TakenRoll);   // 실패 시 장부 복구
		return;
	}

	UKernelItemInstance* Inst = NewObject<UKernelItemInstance>(P);
	Inst->InitFromRollResult(TakenRoll);
	Mgr->AddItemToInventory(Inst);   // 기존 서브오브젝트 등록·복제 경로 재사용
}

void AKernelPlayerController::Server_ShareDrop_Implementation(int32 InDropId, const FTransform& Xform)
{
	FKernelItemData SharedRoll;
	if (!ConsumeDrop(InDropId, SharedRoll)) return;
	if (!PickupClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AKernelItemPickup* Shared = GetWorld()->SpawnActor<AKernelItemPickup>(PickupClass, Xform, Params);
	if (!Shared) return;

	Shared->bLocalOnly = false;
	Shared->ApplyRoll(SharedRoll);   // 서버 즉시 반영 + Roll 복제 → 각 클라 OnRep_Roll
}

void AKernelPlayerController::Server_RerollAffix_Implementation(
	int32 SlotIndex, int32 AffixIndex, FName AffixId)
{
	APawn* P = GetPawn();
	UKernelQuickBarComponent* Qbr = P ? P->FindComponentByClass<UKernelQuickBarComponent>() : nullptr;
	if (!Qbr) return;

	const TArray<TObjectPtr<UKernelItemInstance>>& Slots = Qbr->GetSlots();
	if (!Slots.IsValidIndex(SlotIndex)) return;

	UKernelItemInstance* Weapon = Slots[SlotIndex];
	if (!Weapon) return;

	FKernelAppliedAffix NewApplied;
	if (Weapon->TryRerollAffix(AffixIndex, AffixId, NewApplied))
	{
		Client_AffixRerolled(SlotIndex, AffixIndex, NewApplied);
	}
}

void AKernelPlayerController::Client_AffixRerolled_Implementation(
	int32 SlotIndex, int32 AffixIndex, FKernelAppliedAffix NewApplied)
{
	OnAffixRerolled.Broadcast(SlotIndex, AffixIndex, NewApplied);
}

AKernelItemPickup* AKernelPlayerController::GetFocusedLocalPickup() const
{
	APawn* P = GetPawn();
	UKernelInteractionComponent* IC = P ? P->FindComponentByClass<UKernelInteractionComponent>() : nullptr;
	if (!IC) return nullptr;

	AKernelItemPickup* Pickup = Cast<AKernelItemPickup>(IC->CurrentTargetActor.Get());
	return (Pickup && Pickup->bLocalOnly) ? Pickup : nullptr;
}

void AKernelPlayerController::Client_ShowAttackWarning_Implementation(FVector WarnLocation, float Duration)
{
	FKernelVerbMessage Msg;
	Msg.Instigator = this;
	Msg.Magnitude  = Duration;
	// 방향 표시를 원하면 WarnLocation을 담을 필드를 메시지에 추가

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_Warning, Msg);
}

void AKernelPlayerController::OnSharePressed()
{
	if (AKernelItemPickup* P = GetFocusedLocalPickup())
	{
		Server_ShareDrop(P->DropId, P->GetActorTransform());
		P->Destroy();   // 왕복 대기 없이 즉시 제거 — 잠깐 2개로 보이는 것 방지
	}
}

void AKernelPlayerController::Kernel_DumpRanges()
{
	const UKernelItemBalanceSettings* S = GetDefault<UKernelItemBalanceSettings>();
	const UEnum* E = StaticEnum<EItemRarity>();

	UE_LOG(LogTemp, Warning, TEXT("=== 등록된 구간: %d개 ==="), S->RarityChanceRanges.Num());

	for (int32 i = 0; i < E->NumEnums() - 1; ++i)   // -1 은 자동 생성되는 _MAX 제외
	{
		const EItemRarity R = static_cast<EItemRarity>(E->GetValueByIndex(i));
		float Min = 0.f, Max = 0.f;

		if (S->GetChanceRange(R, Min, Max))
		{
			UE_LOG(LogTemp, Warning, TEXT("  %s : %.1f%% ~ %.1f%%"),
				*E->GetNameStringByIndex(i), Min * 100.f, Max * 100.f);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  %s : ★미설정★"), *E->GetNameStringByIndex(i));
		}
	}
}

void AKernelPlayerController::Kernel_TestRoll(int32 Count)
{
	if (!DebugRollItemDef)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugRollItemDef를 BP_PlayerController에서 지정하세요"));
		return;
	}

	TMap<EItemRarity, int32> Weights;
	Weights.Add(EItemRarity::Normal, 40);
	Weights.Add(EItemRarity::Rare,   30);
	Weights.Add(EItemRarity::Unique, 20);
	Weights.Add(EItemRarity::Myth,   10);

	const UEnum* E = StaticEnum<EItemRarity>();

	for (int32 n = 0; n < Count; ++n)
	{
		const FKernelItemData Roll = UKernelAffixRollLibrary::RollItem(DebugRollItemDef, Weights);

		UE_LOG(LogTemp, Warning, TEXT("--- [%d] %s / 접사 %d개 ---"),
			n, *E->GetNameStringByValue((int64)Roll.Rarity), Roll.Affixes.Num());

		for (const FKernelAppliedAffix& A : Roll.Affixes)
		{
			if (!A.Affix) continue;
			UE_LOG(LogTemp, Warning, TEXT("      %s : %.1f%%"),
				*A.Affix->AffixId.ToString(), A.RolledChance * 100.f);
		}
	}
}