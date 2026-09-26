// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/KernelProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Affixes/KernelAffixCombatLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKernelProjectileBase::AKernelProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
	SetReplicateMovement(false);

	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	CollisionComp->InitSphereRadius(6.f);
	CollisionComp->SetCollisionProfileName(FName("NoCollision"));
	SetRootComponent(CollisionComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailFX = CreateDefaultSubobject<UNiagaraComponent>("TrailFX");
	TrailFX->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->SetUpdatedComponent(CollisionComp);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 10.f;
}

void AKernelProjectileBase::InitAsTracer(float InSpeed, float InLifeSpan)
{
	bDamaging = false;

	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (ProjectileMovement && InSpeed > 0.f)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
	}
	InitialLifeSpan = InLifeSpan;
}

void AKernelProjectileBase::InitAsDamaging(
	UAbilitySystemComponent* InstigatorASC,
	const FGameplayEffectSpecHandle& InSpec,
	UKernelItemInstance* InWeapon, float InSpeed)
{
	CachedASC = InstigatorASC;
	bDamaging = true;
	DamageSpec = InSpec;
	SourceWeapon = InWeapon;

	if (CollisionComp)
	{
		CollisionComp->SetCollisionProfileName(FName("Projectile"));
	}
	if (ProjectileMovement && InSpeed > 0.f)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
	}
}

void AKernelProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Shooter = GetInstigator())
	{
		if (CollisionComp) CollisionComp->IgnoreActorWhenMoving(Shooter, true);
	}

	// 판정은 서버 + 데미지 발사체일 때만
	if (bDamaging && HasAuthority() && CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AKernelProjectileBase::OnProjectileHit);
	}
}

void AKernelProjectileBase::OnProjectileHit(UPrimitiveComponent*, AActor* OtherActor,
											UPrimitiveComponent*, FVector, const FHitResult& Hit)
{
	if (!HasAuthority() || bHitProcessed) return;
	if (OtherActor == GetInstigator()) return;

	bHitProcessed = true; // 한 프레임에 두 번 히트하는 것 방지
	HandleHit(Hit);
}

void AKernelProjectileBase::HandleHit(const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleHit"))
	if (DamageSpec.IsValid())
	{
		UAbilitySystemComponent* TargetASC = nullptr;
		
		DamageSpec.Data->GetContext().AddHitResult(Hit);

		if (ExplosionRadius > 0.f)
		{
			TArray<FOverlapResult> Overlaps;
			GetWorld()->OverlapMultiByChannel(Overlaps, Hit.ImpactPoint, FQuat::Identity,
				ECC_Pawn, FCollisionShape::MakeSphere(ExplosionRadius));

			TSet<AActor*> DamagedActor;
			for (const FOverlapResult& Result : Overlaps)
			{
				AActor* Target = Result.GetActor();
				if (!Target || DamagedActor.Contains(Target)) continue;
				DamagedActor.Add(Target);

				if (Target)
				{
					TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
				}
			}
		}
		else if (Hit.GetActor())
		{
			TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());
		}
		
		if (TargetASC == nullptr) return;
		if (TargetASC->HasMatchingGameplayTag(TAG_Status_Death_Dying)) return;
		
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
		UKernelAffixCombatLibrary::ApplyAffixOnHit(CachedASC, TargetASC, SourceWeapon, Hit);
		
		FGameplayCueParameters P;
		P.Location = Hit.Location;
		TargetASC->ExecuteGameplayCue(TAG_GameplayCue_Hit_Health, P);
	}
	
	SetLifeSpan(0.1f);
	Destroy();
}
