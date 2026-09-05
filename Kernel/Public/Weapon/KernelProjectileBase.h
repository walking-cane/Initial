#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "KernelProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UKernelItemInstance;

UCLASS()
class KERNEL_API AKernelProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AKernelProjectileBase();
	
	/** 연출 전용 예광탄 — 충돌·데미지 없음. 히트스캔이 사용 */
	void InitAsTracer(float InSpeed, float InLifeSpan = 1.f);

	/** 실제 발사체 — 충돌 시 데미지. 투사체 무기가 사용 (서버에서만 호출) */
	void InitAsDamaging(
		UAbilitySystemComponent* InstigatorASC,
		const FGameplayEffectSpecHandle& InSpec, 
		UKernelItemInstance* InWeapon, 
		float InSpeed);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void HandleHit(const FHitResult& Hit);

	/** 명중 연출을 전 클라이언트에 — 액터가 곧 파괴되므로 Multicast로 미리 알린다 */
	UFUNCTION(Client, Unreliable)
	void Client_PlayImpact(UNiagaraSystem* Impact, const FVector_NetQuantize& Location, const FVector_NetQuantizeNormal& Normal);

	UPROPERTY(VisibleAnywhere) TObjectPtr<USphereComponent> CollisionComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> MeshComp;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UNiagaraComponent> TrailFX;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile") TObjectPtr<UNiagaraSystem> ImpactFX;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile") TObjectPtr<USoundBase> ImpactSound;

	/** 0보다 크면 반경 데미지 */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile") float ExplosionRadius = 0.f;

private:
	/** 서버에만 존재 — 판정이 서버에서만 일어나므로 복제 불필요 */
	FGameplayEffectSpecHandle DamageSpec;

	UPROPERTY() TObjectPtr<UKernelItemInstance> SourceWeapon;
	UPROPERTY() TObjectPtr<UAbilitySystemComponent> CachedASC;

	bool bDamaging = false;
	bool bHitProcessed = false;
};