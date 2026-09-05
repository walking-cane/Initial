#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KernelMenuFlowSubsystem.generated.h"

class ULevelStreamingDynamic;

UCLASS(Config = Game)
class KERNEL_API UKernelMenuFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** 리슨 서버로 게임 맵을 연다 — 이 플레이어가 호스트가 됨 */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void HostGame();

	/** (4단계에서 구현) 세션 검색 */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void FindGames();
	
protected:
	// Config/DefaultGame.ini 의 [/Script/Kernel.MenuFlowSubsystem] 섹션에서 오버라이드 가능
	UPROPERTY(Config)
	FSoftObjectPath GameMapPath = FSoftObjectPath(TEXT("/Game/Level/BaseLevel.BaseLevel"));
};