
#pragma once

#include "KernelItemFragment.h"
#include "Item/KernelItemTypes.h"
#include "UI/HUD/KernelCrossHairStyle.h"
#include "KernelItemFragment_Weapon.generated.h"

class UKernelAffixDefinition;
class UKernelAbilitySet;

UCLASS()
class KERNEL_API UKernelItemFragment_Weapon : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> WeaponLayer1P;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> WeaponLayer3P;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	FKernelCrosshairStyle CrosshairStyle;
	
	/** 이 목록에서 정확히 1개만 뽑힌다 (속성 등 배타적 옵션) */
	UPROPERTY(EditDefaultsOnly, Category = "Affix")
	TArray<TObjectPtr<UKernelAffixDefinition>> ExclusiveAffixPool;

	/** 여러 개 뽑힐 수 있는 일반 옵션 풀 */
	UPROPERTY(EditDefaultsOnly, Category = "Affix")
	TArray<TObjectPtr<UKernelAffixDefinition>> CommonAffixPool;

	/** 이 무기에서 제외할 접사 — 공유 풀을 쓸 때 무기별로 빼기용 (선택) */
	UPROPERTY(EditDefaultsOnly, Category = "Affix")
	TArray<TObjectPtr<UKernelAffixDefinition>> ExcludedAffixes;

	/** 등급 → 일반 옵션 개수. 배타 옵션은 항상 1개 */
	UPROPERTY(EditDefaultsOnly, Category = "Affix")
	TMap<EItemRarity, FInt32Interval> CommonAffixCountByRarity;
};