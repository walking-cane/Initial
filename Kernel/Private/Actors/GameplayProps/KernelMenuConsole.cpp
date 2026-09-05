// MenuConsole.cpp
#include "Actors/GameplayProps/KernelMenuConsole.h"

#include "Components/BoxComponent.h"
#include "Game/KernelGameState.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelPlayerState.h"

AKernelMenuConsole::AKernelMenuConsole()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ConsoleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ConsoleBodyMesh");
	ConsoleMesh->SetCustomDepthStencilValue(5);
	SetRootComponent(ConsoleMesh);

	OverlapCollision = CreateDefaultSubobject<UBoxComponent>("CheckCollision");
	OverlapCollision->SetupAttachment(ConsoleMesh);
}

void AKernelMenuConsole::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && OverlapCollision)
	{
		// Trigger server only.
		OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		OverlapCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}
}

void AKernelMenuConsole::OnOverlapBegin(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		UE_LOG(LogTemp,Error,TEXT("[BlockDoor] No Pawn"));
		return;
	}
	
	AController* Controller = Pawn->Controller;
	if (!Controller)
	{
		UE_LOG(LogTemp,Error,TEXT("[BlockDoor] No Controller"));
		return;
	}
	APlayerState* PS = Controller->PlayerState;
	if (!PS) return;
	
	GatheredPlayerStates.Add(PS);
}

void AKernelMenuConsole::OnOverlapEnd(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		UE_LOG(LogTemp,Error,TEXT("[MenuConsole] No Pawn"));
		return;
	}
	
	AController* Controller = Pawn->Controller;
	if (!Controller)
	{
		UE_LOG(LogTemp,Warning,TEXT("[MenuConsole] No Controller"));
		return;
	}
	APlayerState* PS = Controller->PlayerState;
	if (!PS) return;
	
	GatheredPlayerStates.Remove(PS);
}

int32 AKernelMenuConsole::CountExistPlayers() const
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

bool AKernelMenuConsole::CheckAllPlayersGathered() const
{
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return false;

	int32 Players = CountExistPlayers();
	if (Players == 0) return false;
	
	if (GatheredPlayerStates.Num() >= Players)
	{
		return true;
	}
	return false;
}

void AKernelMenuConsole::OnInteract(APawn* InteractingPawn)
{
	if (!InteractingPawn || !InteractingPawn->IsLocallyControlled()) return;
	
	FKernelToastMessage Message;
	Message.IsWarning = true;
	
	if (!InteractingPawn->HasAuthority())
	{
		Message.ToastMessage = FText::FromString(TEXT("서버장만 콘솔을 열 수 있음"));
		
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
			TAG_UI_ToastMessage, Message);
		
		return;
	}
	
	if (!CheckAllPlayersGathered())
	{
		Message.ToastMessage = FText::FromString(TEXT("모든 플레이어가 근처에 있어야 함"));
			
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		TAG_UI_ToastMessage, Message);
		return;	
	}
	
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS || GS->GetGamePhase() != EKernelGamePhase::Boarding) return;
	
	GS->SetGamePhase(EKernelGamePhase::MapSelect);
	
	UE_LOG(LogTemp,Log,TEXT("[MenuConsole] ALl Players Ready. Set Phase to MapSelect"));
}

void AKernelMenuConsole::OnFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	ConsoleMesh->SetRenderCustomDepth(true);
	
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_OnFocus, FKernelInteractionMessage());
}

void AKernelMenuConsole::OnEndFocus(APawn* FocusingPawn)
{
	if (!FocusingPawn || !FocusingPawn->IsLocallyControlled()) return;
	
	ConsoleMesh->SetRenderCustomDepth(false);
	
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_EndFocus, FKernelInteractionMessage());
}
