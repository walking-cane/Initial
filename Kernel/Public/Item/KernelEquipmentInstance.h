#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h" // GrantedAbilityHandles를 위한 필수 헤더
#include "KernelEquipmentInstance.generated.h"

class UKernelItemInstance;
class AActor;

UCLASS(BlueprintType, Blueprintable)
class KERNEL_API UKernelEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	// 이 장비의 원본이 되는 인벤토리 데이터 포인터 (리플리케이션 필수)
	UPROPERTY(Replicated)
	TObjectPtr<UKernelItemInstance> InstigatorItem;

	// [누락되었던 핵심 변수] 부여된 어빌리티 핸들들을 추적하여 해제 시 사용합니다.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	float LastFireTime = 0.0f;

	// 장착 시 호출 (GAS 어빌리티 부여 및 물리 액터 스폰)
	virtual void OnEquipped(AActor* Owner);

	// 장착 해제 시 호출 (GAS 어빌리티 회수 및 물리 액터 파괴)
	virtual void UnEquipped();

	// 멀티플레이어 환경에서 객체 동기화를 위해 반드시 필요합니다.
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetAudioComponent(UAudioComponent* InComp) 
	{ 
		CachedAudioComp = InComp; 
	}
    
	UAudioComponent* GetAudioComponent() const 
	{ 
		return CachedAudioComp; 
	}

protected:
	// 스폰된 물리 액터들을 추적하여 해제 시 파괴할 수 있게 합니다.
	UPROPERTY()
	TObjectPtr<AActor> SpawnedEquippedActor;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> CachedAudioComp;
};