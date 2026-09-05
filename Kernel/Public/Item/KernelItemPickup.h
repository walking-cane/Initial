// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/KernelItemTypes.h"
#include "Interaction/KernelInteractionInterface.h"
#include "KernelItemPickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UKernelItemFragment_Cosmetic;
class UKernelItemInstance;

static uint8 GetStencilForRarity(EItemRarity Rarity)
{
	switch (Rarity)
	{
		case EItemRarity::Myth:   return 4;
		case EItemRarity::Unique: return 3;
		case EItemRarity::Rare:   return 2;
		case EItemRarity::Normal: return 1;
		default:                  return 0;
	}
}

UCLASS()
class KERNEL_API AKernelItemPickup : public AActor, public IKernelInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AKernelItemPickup();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Roll 데이터로 인스턴스 생성 + 외형 구성. 로컬/서버 어디서 불러도 동일 동작 */
	void ApplyRoll(const FKernelItemData& InRoll);
	
	// 인터페이스 구현부
	virtual void OnInteract(APawn* Pawn) override;
	virtual void OnFocus(APawn* Pawn) override;
	virtual void OnEndFocus(APawn* Pawn) override;
	
	virtual FText GetInteractName() override;
	virtual FText GetInteractDescription() override;
	virtual UTexture2D* GetInteractIcon() override;
	virtual FVector GetInteractLocation() override;
	virtual TArray<FKernelAffixDetail> GetInteractDetails() override;
	
	/** true면 이 머신에만 존재하는 개인 드롭 */
	UPROPERTY(BlueprintReadOnly)
	bool bLocalOnly = false;

	/** 서버 장부와 대조할 ID (로컬 드롭에만 유효) */
	UPROPERTY(BlueprintReadOnly)
	int32 DropId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UKernelItemInstance> ItemInstance;

	UPROPERTY()
	TObjectPtr<const UKernelItemFragment_Cosmetic> CosmeticFrag;
	
protected:
	virtual void BeginPlay() override;
	
	/** 공유 픽업에서만 복제됨 — 도착 시 OnRep이 외형을 구성 */
	UPROPERTY(ReplicatedUsing = OnRep_Roll)
	FKernelItemData Roll;

	UFUNCTION()
	void OnRep_Roll() { ApplyRoll(Roll); }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) 
	TObjectPtr<USkeletalMeshComponent> SpawnMeshComp1;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> RarityFX1;
	
	UPROPERTY(EditDefaultsOnly, Config)
	TMap<EItemRarity, TSoftObjectPtr<UNiagaraSystem>> RarityFXAssets;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float MoveAmount = 0.1f;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MovedDistance = 0.f;
	
	FVector OriginVector;
};
