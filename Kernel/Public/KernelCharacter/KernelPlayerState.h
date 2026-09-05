// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"
#include "KernelPlayerState.generated.h"

class UKernelCharacterDefinition;
class UKernelCombatAttributeSet;
class UKernelAbilitySystemComponent;
class UKernelAttributeSet;

DECLARE_MULTICAST_DELEGATE(FOnLobbySelectionChanged);
/**
 * 
 */
UCLASS()
class KERNEL_API AKernelPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AKernelPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return KernelASC; }
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void SetCharacterDefinition(UKernelCharacterDefinition* InDef); // 서버 전용
	
	UKernelAbilitySystemComponent* GetKernelAbilitySystemComponent() const { return KernelASC; }
	UKernelHealthAttributeSet* GetHealthSet() const { return HealthSet; }
	UKernelCombatAttributeSet* GetCombatSet() const { return CombatSet; }
	UKernelCharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	int32 GetSelectedCharacterIndex() const { return SelectedCharacterIndex; }
	
	FOnLobbySelectionChanged OnLobbySelectionChanged;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeCharacter(int32 CharacterIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_VoteMap(int32 MapIndex);
	
	/** 클라이언트가 자기 로컬 세이브의 해금 목록을 서버에 신고 */
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReportUnlockedCharacters(const TArray<FName>& UnlockedIds);

	/** 이 플레이어가 해당 캐릭터를 해금했는가 (서버 검증 + 로컬 UI 잠금 표시 양쪽에서 사용) */
	bool IsCharacterUnlocked(FName CharacterId) const { return UnlockedCharacterIds.Contains(CharacterId); }
	
	int32 GetVotedMapIndex() const { return VotedMapIndex; }
	
	FOnLobbySelectionChanged OnMapVoteChanged; // 기존 델리게이트 타입 재사용
	FOnLobbySelectionChanged OnUnlocksChanged; // UI 갱신용 (기존 델리게이트 타입 재사용)
	
	bool IsBoarded() const { return bIsBoarded; }
	void SetBoarded(bool Value); // 서버 전용
	
	/** 플레이어 누적 데미지 업데이트 */
	void SetTotalDamageDealt(float NewValue);
	float GetTotalDamageDealt();
	
	UFUNCTION()
	void OnRep_TotalDamageDealt();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY() TObjectPtr<UKernelAbilitySystemComponent> KernelASC;
	UPROPERTY() TObjectPtr<UKernelHealthAttributeSet> HealthSet;
	UPROPERTY() TObjectPtr<UKernelCombatAttributeSet> CombatSet;
	
	UPROPERTY(Replicated)
	TObjectPtr<UKernelCharacterDefinition> CharacterDefinition; // 임시로 기본 캐릭터 Def 를 설정함 (TODO : 런칭 이전에 비우기)
	UPROPERTY(ReplicatedUsing = OnRep_SelectedCharacter)
	int32 SelectedCharacterIndex = 0;

	UPROPERTY(ReplicatedUsing = OnRep_VotedMap)
	int32 VotedMapIndex = INDEX_NONE;
	
	UPROPERTY(Replicated)
	bool bIsBoarded = false;

	UFUNCTION()
	void OnRep_VotedMap();
	
	UFUNCTION()
	void OnRep_SelectedCharacter();
	
	// KernelPlayerState.h에 선언 추가
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
	/** 서버가 보관하는 이 플레이어의 해금 목록.
	 *  OwnerOnly로 복제 — 남의 해금 상태는 아무도 볼 필요가 없으므로 대역폭 절약 */
	UPROPERTY(ReplicatedUsing = OnRep_UnlockedCharacters)
	TArray<FName> UnlockedCharacterIds;

	UFUNCTION() 
	void OnRep_UnlockedCharacters();
	
	UPROPERTY(ReplicatedUsing = OnRep_TotalDamageDealt) 
	float TotalDamageDealt = 0;
		
	void BroadcastDamageTotal();
	
private:
	double LastMapPingTime = 0.0; // 서버 전용 — 핑 스팸 스로틀
};
