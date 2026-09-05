// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GameplayProps/KernelBlockEntrance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AKernelBlockEntrance::AKernelBlockEntrance()
{
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>("BlockMesh");
	BlockMesh->SetCustomDepthStencilValue(5);
	SetRootComponent(BlockMesh);
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AKernelBlockEntrance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AKernelBlockEntrance, GatheredPlayerStates);
	DOREPLIFETIME(AKernelBlockEntrance, bOpening);
}

void AKernelBlockEntrance::OnInteract(APawn* InteractingPawn)
{
	if (!HasAuthority() || !InteractingPawn) return;
	
	AController* Controller = InteractingPawn->GetController();
	if (!Controller)
	{
		UE_LOG(LogTemp,Warning,TEXT("[BlockDoor] No Controller"));
		return;
	}
	
	APlayerState* PS = Controller->PlayerState;
	
	if (GatheredPlayerStates.Contains(PS))
	{
		GatheredPlayerStates.Remove(PS);
	}
	else
	{
		GatheredPlayerStates.Add(PS);
	}
	
	OnRep_GatheredPlayerStates();
	TryToOpenDoor();
}

int32 AKernelBlockEntrance::CountExistPlayers() const
{
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return 0;
	
	/** 관전자, 스폰 중 (GetPawn), 봇 (GetOwner) 거르기 */
	int32 ExistPlayers = 0;
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;
		if (PS->IsSpectator()) continue;
		if (!PS->GetPawn()) continue;
		
		ExistPlayers++;
	}
	
	return ExistPlayers;
}

void AKernelBlockEntrance::TryToOpenDoor()
{
	if (!HasAuthority() || bOpening) return;

	int32 Players = CountExistPlayers();
	if (Players == 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("[BlockDoor] No one Exist"))
		return;
	}
	
	if (GatheredPlayerStates.Num() >= Players)
	{
		OpenDoor();
	}
}

void AKernelBlockEntrance::OpenDoor()
{
	bOpening = true;
	OnRep_Opening();
}

void AKernelBlockEntrance::OnFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	if (bOpening) return;
	BlockMesh->SetRenderCustomDepth(true);
}

void AKernelBlockEntrance::OnEndFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	BlockMesh->SetRenderCustomDepth(false);
}

void AKernelBlockEntrance::OnRep_GatheredPlayerStates()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	APlayerState* PS = PC ? PC->PlayerState : nullptr;
	
	FKernelReadyCountMessage Message;
	Message.ReadyCount = GatheredPlayerStates.Num();
	Message.TotalCount = CountExistPlayers();
	Message.bIsLocalPlayerReady = GatheredPlayerStates.Contains(PS);

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_Interact_PlayerReady, Message);
}

void AKernelBlockEntrance::OnRep_Opening()
{
	if (!bOpening) return;
	if (APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		Camera->StartCameraShake(OpeningCameraShake);
	}
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_Interact_EndFocus, FKernelInteractionMessage());
	
	if (BlockMesh)
	{
		BlockMesh->SetCollisionProfileName("BlockAll"); // 문 열리는 중에도 콜리전 유지.
	}
	
	PlayOpenSequence();
}

void AKernelBlockEntrance::FinishOpen()
{
	SetActorEnableCollision(false);
}