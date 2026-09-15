// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KernelGameState.h"
#include "GameFramework/GameModeBase.h"
#include "KernelGameModeBase.generated.h"

class UKernelArtifactDefinition;
struct FKernelArtifactPoolEntry;
class UKernelArtifactPool;
class AKernelPlayerState;
/**
 * 
 */
UCLASS()
class KERNEL_API AKernelGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AKernelGameModeBase();
	
	virtual void InitGameState() override;
	
	/** 호스트의 START — 전원 픽 완료 시 게임 페이즈 진입 */
	void ConfirmMapAndBeginCharacterSelect();
	void BeginDeparture();
	
	/** 전원에게 제안. 플레이어마다 독립적으로 굴린다 */
	UFUNCTION(BlueprintCallable, Category = "Artifact")
	void OfferArtifactsToAll(int32 ChoiceCount = 3);

	/** 한 명에게만 제안 */
	void OfferArtifactsToPlayer(AKernelPlayerState* PS, int32 ChoiceCount);

protected:
	/** 이 맵의 시작 페이즈 — 허브 맵 BP는 Lobby, 게임 맵용 자식 BP는 Playing으로 지정 */
	UPROPERTY(EditDefaultsOnly, Category = "Flow")
	EKernelGamePhase InitialPhase = EKernelGamePhase::Boarding;
	
	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	TObjectPtr<UKernelArtifactPool> GlobalPool;

	void GatherPool(const UKernelArtifactPool* Pool,
					TArray<FKernelArtifactPoolEntry>& Out,
					TSet<const UKernelArtifactPool*>& Visited) const;

	/** 가중치 기반으로 Count개를 중복 없이 뽑는다 */
	TArray<UKernelArtifactDefinition*> PickWeighted(
		TArray<FKernelArtifactPoolEntry>& Candidates, int32 Count) const;
	
private:
	int32 NextOfferId = 0;
};
