// KernelSaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KernelSaveGame.generated.h"

UCLASS()
class KERNEL_API UKernelSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** 세이브 포맷 버전. 나중에 구조가 바뀌면 로드 시 이 값으로 마이그레이션 분기. */
	UPROPERTY()
	int32 SaveVersion = 1;

	/** 해금된 캐릭터들의 CharacterId 목록 (인덱스가 아니라 ID — 로스터 순서 변경에 안전) */
	UPROPERTY()
	TArray<FName> UnlockedCharacterIds;

	/** 마지막에 고른 캐릭터 — 다음 판 기본 선택값으로 쓰면 편의성이 올라감 */
	UPROPERTY()
	FName LastSelectedCharacterId;

	/** 앞으로 추가될 진행도 (통화, 무기 강화 레벨 등)도 전부 여기에 */
	UPROPERTY()
	int32 Currency = 0;
};