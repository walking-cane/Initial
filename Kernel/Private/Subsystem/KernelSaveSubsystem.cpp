// KernelSaveSubsystem.cpp
#include "Subsystem/KernelSaveSubsystem.h"
#include "Game/KernelSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString UKernelSaveSubsystem::SlotName = TEXT("KernelProfile");

void UKernelSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadOrCreate(); // 게임 시작과 동시에 프로필 확보
}

void UKernelSaveSubsystem::LoadOrCreate()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		CurrentSave = Cast<UKernelSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	}

	// 파일이 없거나 로드 실패(포맷 손상 등) → 새 프로필 생성
	if (!CurrentSave)
	{
		CurrentSave = Cast<UKernelSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UKernelSaveGame::StaticClass()));

		// 첫 실행: 기본 캐릭터를 열어준다
		CurrentSave->UnlockedCharacterIds = DefaultUnlockedIds;
		SaveNow();
		UE_LOG(LogTemp, Log, TEXT("[Save] 새 프로필 생성"));
	}
}

bool UKernelSaveSubsystem::IsCharacterUnlocked(FName CharacterId) const
{
	if (CharacterId.IsNone()) return false;
	return CurrentSave && CurrentSave->UnlockedCharacterIds.Contains(CharacterId);
}

void UKernelSaveSubsystem::UnlockCharacter(FName CharacterId)
{
	if (!CurrentSave || CharacterId.IsNone()) return;
	if (CurrentSave->UnlockedCharacterIds.Contains(CharacterId)) return; // 중복 방지

	CurrentSave->UnlockedCharacterIds.Add(CharacterId);
	SaveNow(); // 해금은 놓치면 안 되는 이벤트라 즉시 저장
}

void UKernelSaveSubsystem::SaveNow()
{
	if (!CurrentSave) return;
	// 대용량이 되면 AsyncSaveGameToSlot으로 교체 (지금 규모는 동기로 충분)
	UGameplayStatics::SaveGameToSlot(CurrentSave, SlotName, UserIndex);
}

const TArray<FName>& UKernelSaveSubsystem::GetUnlockedCharacterIds() const
{
	static const TArray<FName> Empty;
	return CurrentSave ? CurrentSave->UnlockedCharacterIds : Empty;
}