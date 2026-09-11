#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "KernelCosmeticComponent.generated.h"

class UAnimInstance;
class UAnimMontage;
class USkeletalMeshComponent;

/**
 * 복제되는 부착 정보. 소프트 클래스는 서버에서 로드를 끝낸 뒤
 * 하드 TSubclassOf 로 넣어서 보낸다.
 */
USTRUCT()
struct FKernelWeaponAttachEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY()
	FName AttachSocket1P = NAME_None;

	UPROPERTY()
	FName AttachSocket3P = NAME_None;

	UPROPERTY()
	FTransform AttachTransform1P = FTransform::Identity;
	
	UPROPERTY()
	FTransform AttachTransform3P = FTransform::Identity;
};

UCLASS(ClassGroup = (Kernel), meta = (BlueprintSpawnableComponent))
class KERNEL_API UKernelCosmeticComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKernelCosmeticComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** [서버] 장착 무기의 부착 규칙을 통째로 교체한다. 빈 배열이면 해제. */
	void SetWeaponAttachEntries(const TArray<FKernelWeaponAttachEntry>& NewEntries);

	/** [서버] 무기 해제. SetWeaponAttachEntries({}) 와 동일. */
	void ClearWeaponAttachEntries();

	void ApplyWeaponLayer(TSubclassOf<UAnimInstance> Layer1P, TSubclassOf<UAnimInstance> Layer3P);
	void PlayEquipMontage(UAnimMontage* Montage);

protected:
	UFUNCTION()
	void OnRep_WeaponAttachEntries();

	UFUNCTION()
	void OnRep_WeaponLayers();

	UFUNCTION()
	void OnRep_EquipMontage();

	/** 현재 엔트리 기준으로 액터를 만들어 붙인다. 로컬 전용, 복제 없음. */
	void SpawnWeaponActors();

	/** 스폰해둔 무기 액터를 전부 파괴한다. */
	void DestroySpawnedWeaponActors();

	AActor* SpawnAndAttachWeapon(
		const FKernelWeaponAttachEntry& Entry,
		USkeletalMeshComponent* AttachTarget,
		FName SocketName,
		bool bOnlyOwnerSee,
		bool bOwnerNoSee);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponAttachEntries)
	TArray<FKernelWeaponAttachEntry> WeaponAttachEntries;

	/** 이 머신에서 스폰한 무기 액터들. 복제되지 않는 순수 로컬 상태. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> SpawnedWeaponActors;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponLayers)
	TSubclassOf<UAnimInstance> CurrentLayer1P;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponLayers)
	TSubclassOf<UAnimInstance> CurrentLayer3P;

	/** 몽타주 본체는 그냥 복제하고, 재생 트리거는 카운터의 OnRep 이 담당한다. */
	UPROPERTY(Replicated)
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(ReplicatedUsing = OnRep_EquipMontage)
	uint8 EquipMontageCounter = 0;
};