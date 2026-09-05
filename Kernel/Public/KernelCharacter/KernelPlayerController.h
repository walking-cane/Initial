// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "Item/KernelItemTypes.h"
#include "KernelPlayerController.generated.h"

class AKernelRewardCrate;
enum class EKernelGamePhase : uint8;
class UKernelItemInstance;
class AKernelItemPickup;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAffixRerolled, int32, int32, const FKernelAppliedAffix&);

USTRUCT(BlueprintType)
struct FKernelCrateOfferMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FKernelItemData> Options;
	
	bool bIsOpen = false;
};

UCLASS()
class KERNEL_API AKernelPlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TObjectPtr<UKernelItemDefinition> DebugRollItemDef;

	UFUNCTION(Exec) void Kernel_DumpRanges();
	UFUNCTION(Exec) void Kernel_TestRoll(int32 Count = 5);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void ReportUnlocksToServer();
	
	UFUNCTION(Exec) void Save_Unlock(FName CharacterId);
	UFUNCTION(Exec) void Save_Lock(FName CharacterId);
	UFUNCTION(Exec) void Save_ResetProfile();
	UFUNCTION(Exec) void Save_Dump();
	
	/** [서버] 이 플레이어에게 드롭을 발급하고 클라에 스폰 지시. DropId 반환 */
	int32 GrantLocalDrop(const FKernelItemData& Roll, const FTransform& Xform);

	// [ClientRPC] 클라이언트에게 아이템을 로컬로 드랍하도록 명령
	UFUNCTION(Client, Reliable)
	void Client_SpawnLocalDrop(int32 InDropId, const FKernelItemData& InRoll, const FTransform& Xform);

	// [ServerRPC] 클라이언트가 서버에게 로컬 아이템 획득을 보고, 공유된 서버 아이템은 ItemManager로 직접 처리.
	UFUNCTION(Server, Reliable)
	void Server_TakeDrop(int32 InDropId);

	// [ServerRPC] 클라이언트가 본인 화면의 로컬 아이템을 모든 클라이언트에게 공유하도록 서버에게 요청
	UFUNCTION(Server, Reliable)
	void Server_ShareDrop(int32 InDropId, const FTransform& Xform);
	
	UFUNCTION(Server, Reliable)
	void Server_RerollAffix(int32 SlotIndex, int32 AffixIndex, FName AffixId);

	UFUNCTION(Client, Reliable)
	void Client_AffixRerolled(int32 SlotIndex, int32 AffixIndex, FKernelAppliedAffix NewApplied);

	UFUNCTION(Client, Reliable)
	void Client_ShowAttackWarning(FVector WarnLocation, float Duration);
	
	/** 공유 키 입력 핸들러 */
	void OnSharePressed();
	
	FOnAffixRerolled OnAffixRerolled;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	TSubclassOf<AKernelItemPickup> PickupClass;

	AKernelItemPickup* GetFocusedLocalPickup() const;
	
	// 캐릭터 선택 후 출발하는 연출 시퀀스.
	UFUNCTION(BlueprintImplementableEvent, Category = "Flow")
	void PlayDepartureSequence();
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ChooseWeaponWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> ChooseWeaponWidget;
	
private:
	/** [서버 전용] 아직 소비되지 않은 이 플레이어의 드롭들 */
	TMap<int32, FKernelItemData> PendingDrops;
	int32 NextDropId = 1;

	bool ConsumeDrop(int32 InDropId, FKernelItemData& OutRoll);
	
	void HandleGamePhase(EKernelGamePhase Phase);
	void TryBindToGameState();
};
