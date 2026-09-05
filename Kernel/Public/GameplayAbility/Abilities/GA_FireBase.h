#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_FireBase.generated.h"

class AKernelProjectileBase;
class IKernelCosmeticInterface;
class UKernelHeroWeaponComponent;
class UKernelEquipmentInstance;
class UCurveVector;
class UAudioComponent;

UCLASS()
class KERNEL_API UGA_FireBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_FireBase();
	
	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	virtual void Fire();
	virtual void AddRecoilKick();
	
	UFUNCTION() 
	virtual void StartFireLoop();
	
	UPROPERTY(VisibleAnywhere, Category = "WeaponInfo") TObjectPtr<UAnimMontage> FireMontage;
	UPROPERTY(VisibleAnywhere, Category = "WeaponInfo") TObjectPtr<UCurveVector> RecoilCurve;
	UPROPERTY(VisibleAnywhere, Category = "WeaponInfo") float FireDelay;
	UPROPERTY(VisibleAnywhere, Category = "WeaponInfo") float Damage;
	UPROPERTY(EditDefaultsOnly) FGameplayTag FireCueTag;
	UPROPERTY(EditDefaultsOnly) float FireSpeed = 1000.f;
	
	// Caching Properties
	UPROPERTY() TObjectPtr<AActor> OwnerActor;
	UPROPERTY() TObjectPtr<UKernelEquipmentInstance> EquipInst;
	
	UPROPERTY() TObjectPtr<UKernelHeroWeaponComponent> CachedWeaponComp;
	UPROPERTY() TObjectPtr<UAbilitySystemComponent> CachedASC;
	UPROPERTY() float CachedRecoilKick;
	UPROPERTY() float CachedMaxRecoilKick;
	
	UPROPERTY() TObjectPtr<USkeletalMeshComponent> WeaponMesh1P;
	UPROPERTY() TObjectPtr<USkeletalMeshComponent> WeaponMesh3P;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ProjectileClassWithoutCollision;
	
	FTimerHandle FireTimerHandle;
	void InitializeWeapon();
	
	void OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	bool CalculateAimPoint(FVector& OutMuzzleLocation, FRotator& OutSpawnRotation) const;

	/** 총구 화염 — 로컬 연출만. 예광탄 스폰은 히트스캔이 담당 */
	void PlayMuzzleCue();

	UPROPERTY(EditDefaultsOnly) TSubclassOf<AKernelProjectileBase> TracerClass;      // 타입 좁힘
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AKernelProjectileBase> ProjectileClass;  // 실제 발사체
	UPROPERTY(EditDefaultsOnly) float ProjectileSpeed = 5000.f;
	UPROPERTY(EditDefaultsOnly) float TracerSpeed = 12000.f;
};