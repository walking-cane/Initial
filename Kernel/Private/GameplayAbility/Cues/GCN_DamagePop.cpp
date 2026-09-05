// GCN_DamagePop.cpp
#include "GameplayAbility/Cues/GCN_DamagePop.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

bool UGCN_DamagePop::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UE_LOG(LogTemp,Warning,TEXT("[GA_FireBase] Instigator : %s"), *MyTarget->GetName());
	
	UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	if (!World || !DamageNumberSystemAsset) return false;
	if (World->GetNetMode() == NM_DedicatedServer) return false;

	// [핵심] 위치는 EffectContext의 HitResult에서 꺼낸다.
	// GE에 붙은 큐는 Parameters.Location을 채워주지 않는다 — 히트 정보는 컨텍스트에 실려 온다.
	FVector BaseLocation = Parameters.Location;
	if (const FHitResult* CtxHit = Parameters.EffectContext.GetHitResult())
	{
		BaseLocation = CtxHit->ImpactPoint;
	}
	else if (MyTarget)
	{
		// 컨텍스트에 히트가 없을 때의 최종 폴백: 맞은 액터 머리 위
		BaseLocation = MyTarget->GetActorLocation();
	}

	// 이 머신의 로컬 플레이어와 사수를 비교
	APlayerController* LocalPC = World->GetFirstPlayerController();
	APawn* LocalPawn = LocalPC ? LocalPC->GetPawn() : nullptr;
	const bool bIsMyDamage = (Parameters.GetInstigator() == LocalPawn);
	const FLinearColor PopColor = GetDamageColor(Parameters.AggregatedSourceTags);
	
	// 이하 동일 — FinalSpawnLocation 계산에 BaseLocation 사용
	const FVector RandomOffsetXY = FMath::VRand() * FMath::FRandRange(0.0f, 50.0f);
	const FVector RandomOffsetZ = FMath::VRand() * FMath::FRandRange(10.0f, 15.0f);
	const FVector FinalSpawnLocation = BaseLocation + FVector(0.f, 0.f, 10.f) + RandomOffsetXY + RandomOffsetZ;
 
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), DamageNumberSystemAsset, FinalSpawnLocation,
		FRotator::ZeroRotator, FVector(1.f), true, false);
	
	if (NiagaraComp)
	{
		TArray<FVector4> DamageInfoArray;
		DamageInfoArray.Add(FVector4(FinalSpawnLocation, Parameters.RawMagnitude));

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, FName("User.DamageInfo"), DamageInfoArray);
		
		NiagaraComp->SetVariableLinearColor(FName("User.Color"), PopColor);
		
		NiagaraComp->Activate(true);
		
		//UE_LOG(LogTemp, Warning, TEXT("GCN_DamagePop Activate at %s (Mag=%f)"),*FinalSpawnLocation.ToString(), Parameters.RawMagnitude);
	}
	return true;
}

FLinearColor UGCN_DamagePop::GetDamageColor(const FGameplayTagContainer& Tags) const
{
	for (const TPair<FGameplayTag, FLinearColor>& Pair : DamageTypeColors)
	{
		if (Tags.HasTag(Pair.Key)) return Pair.Value;
	}
	return DefaultDamageColor;
}
