#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "KernelEnemyStateComponent.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class KERNEL_API UKernelEnemyStateComponent : 
	public UActorComponent
{
	GENERATED_BODY()

public:
	UKernelEnemyStateComponent();
	
	virtual void BeginPlay() override;
    
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 서버가 상태 태그를 바꿀 때 호출
	void SetEnemyStateTag(FGameplayTag NewStateTag);

protected:
	// [핵심] 넷플리케이션과 RepNotify를 GameplayTag에 그대로 적용합니다.
	UPROPERTY(ReplicatedUsing = OnRep_EnemyStateTag)
	FGameplayTag CurrentEnemyStateTag;

	// 구조체이므로 인자로 이전 태그(OldTag)를 안전하게 받아올 수 있습니다.
	UFUNCTION()
	void OnRep_EnemyStateTag(FGameplayTag OldEnemyStateTag);

	void BroadcastStateChange();
};