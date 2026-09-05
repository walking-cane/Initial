// KernelSaveSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KernelSaveSubsystem.generated.h"

class UKernelSaveGame;

UCLASS()
class KERNEL_API UKernelSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 현재 세이브 데이터 (항상 유효 — 없으면 기본값으로 생성됨) */
	UKernelSaveGame* GetSave() const { return CurrentSave; }

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool IsCharacterUnlocked(FName CharacterId) const;

	/** 해금 처리 + 즉시 저장 (게임 클리어 보상 등에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void UnlockCharacter(FName CharacterId);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveNow();

	/** 해금 목록 전체 — 서버 신고용 */
	const TArray<FName>& GetUnlockedCharacterIds() const;

protected:
	void LoadOrCreate();

	UPROPERTY()
	TObjectPtr<UKernelSaveGame> CurrentSave;

	/** 기본 해금 캐릭터 — 첫 실행 유저가 아무도 못 고르는 사태 방지 */
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	TArray<FName> DefaultUnlockedIds;

	static const FString SlotName;
	static const int32 UserIndex = 0;
};