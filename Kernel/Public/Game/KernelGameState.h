// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KernelGameState.generated.h"

class UKernelCharacterDefinition;
class UKernelMapDefinition;
class UWorld;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMapPing, const FString& /*PlayerName*/, int32 /*MapIndex*/);
DECLARE_MULTICAST_DELEGATE(FOnMapSelectionChanged);

UENUM(BlueprintType)
enum class EKernelGamePhase : uint8
{
	Boarding,        // 허브 배회 + 탑승 진행 중 (전원 탑승 대기)
	MapSelect,       // 전원 탑승 완료, 맵 선택 중
	CharacterSelect, // 맵 확정, 캐릭터 선택 중
	Departing,       // 전원 픽 완료, 출발 연출 중
	Playing,         // 게임 진행
};

UCLASS()
class KERNEL_API AKernelGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	EKernelGamePhase GetGamePhase() const { return GamePhase; }
	const TArray<TObjectPtr<UKernelCharacterDefinition>>& GetCharacterRoster() const { return CharacterRoster; }

	/** 서버 전용 — GameMode가 호출 */
	void SetGamePhase(EKernelGamePhase NewPhase);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EKernelGamePhase);
	FOnGamePhaseChanged OnGamePhaseChanged;
	
	/** 테마 로스터 — UI는 여기서 DisplayName / PreviewImage만 읽는다 */
	const TArray<TObjectPtr<UKernelMapDefinition>>& GetMapRoster() const { return MapRoster; }
	int32 GetHostSelectedMapIndex() const { return HostSelectedMapIndex; }

	/** 서버 전용 — 호스트의 테마 선택 */
	void SetHostSelectedMap(int32 MapIndex);

	/**
	 * 서버 전용 — 선택된 테마의 LevelPool에서 레벨 하나를 추첨한다.
	 * 호출할 때마다 결과가 달라지므로 getter가 아니다. 트래블 직전에 한 번만 호출할 것.
	 * 실패 시 Null 반환.
	 */
	TSoftObjectPtr<UWorld> RollLevelFromSelectedTheme() const;

	/** 서버 → 전원: 핑 이벤트 중계 (상태가 아닌 순간 이벤트라 RPC) */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MapPing(const FString& PlayerName, int32 MapIndex);

	FOnMapPing OnMapPing;                       // 각 클라 로컬에서 위젯이 구독
	FOnMapSelectionChanged OnMapSelectionChanged;
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_GamePhase)
	EKernelGamePhase GamePhase = EKernelGamePhase::Boarding;

	// 로스터는 정적 에셋 목록이라 복제 불필요 — BP 디폴트로 지정, 모든 머신이 같은 걸 가짐
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TArray<TObjectPtr<UKernelCharacterDefinition>> CharacterRoster;

	UFUNCTION()
	void OnRep_GamePhase();
	
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TArray<TObjectPtr<UKernelMapDefinition>> MapRoster;

	// 테마 인덱스. 실제로 뽑힌 레벨은 클라가 알 필요가 없으므로 복제하지 않는다.
	UPROPERTY(ReplicatedUsing = OnRep_HostSelectedMap)
	int32 HostSelectedMapIndex = INDEX_NONE;

	UFUNCTION()
	void OnRep_HostSelectedMap();
	
private:
	UFUNCTION(exec) void Kernel_TotalDamageDealt();
};