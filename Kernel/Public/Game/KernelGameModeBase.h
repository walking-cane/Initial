// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KernelGameState.h"
#include "GameFramework/GameModeBase.h"
#include "KernelGameModeBase.generated.h"

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

protected:
	/** 이 맵의 시작 페이즈 — 허브 맵 BP는 Lobby, 게임 맵용 자식 BP는 Playing으로 지정 */
	UPROPERTY(EditDefaultsOnly, Category = "Flow")
	EKernelGamePhase InitialPhase = EKernelGamePhase::Boarding;
};
