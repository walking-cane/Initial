#include "GameplayAbility/Abilities/GA_Fire_Hitscan.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Weapon/KernelProjectileBase.h"

void UGA_Fire_Hitscan::Fire()
{
	Super::Fire();
	AddRecoilKick();
	
	if (!OwnerActor || !CurrentActorInfo->IsLocallyControlled()) return;
	
	AController* C = OwnerActor->GetInstigatorController();
	if (!C) return;
	
	FVector ViewLocation; 
	FRotator ViewRotation;
	C->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * 10000.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Camera, Params);

	SpawnTracer(HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd);
	
	FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult);

	FScopedPredictionWindow ScopedPrediction(CachedASC, CurrentActivationInfo.GetActivationPredictionKey());

	if (!CurrentActorInfo->IsNetAuthority())
	{
		CachedASC->CallServerSetReplicatedTargetData(
			CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(),
			TargetData,
			FGameplayTag(),
			CachedASC->ScopedPredictionKey);
	}
		
	FGameplayAbilityTargetDataHandle LocalData = TargetData;
	OnTargetDataReady(LocalData);
}

void UGA_Fire_Hitscan::SpawnTracer(const FVector& TargetPoint)
{
	if (!TracerClass) return;

	FVector MuzzleLocation; FRotator SpawnRotation;
	if (!CalculateAimPoint(MuzzleLocation, SpawnRotation)) return;

	// 명중 지점을 향하도록 회전을 다시 계산 (트레이스 결과 기준)
	SpawnRotation = (TargetPoint - MuzzleLocation).Rotation();

	const FTransform SpawnTM(SpawnRotation, MuzzleLocation);
	AKernelProjectileBase* Tracer = GetWorld()->SpawnActorDeferred<AKernelProjectileBase>(
		TracerClass, SpawnTM, OwnerActor, Cast<APawn>(OwnerActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Tracer) return;   // 스폰 실패 방어

	const float Distance = FVector::Dist(MuzzleLocation, TargetPoint);
	Tracer->InitAsTracer(TracerSpeed, FMath::Max(Distance / TracerSpeed, 0.05f));

	Tracer->FinishSpawning(SpawnTM);
}