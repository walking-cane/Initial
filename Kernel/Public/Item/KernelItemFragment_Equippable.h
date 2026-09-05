#pragma once

#include "CoreMinimal.h"
#include "Item/KernelItemFragment.h" // 1단계에서 언급했던 부모 프래그먼트 헤더
#include "KernelItemFragment_Equippable.generated.h"

class UKernelAbilitySet;
class UKernelEquipmentInstance;

// 장착 가능한 아이템(무기, 장비 등)에만 부착되는 프래그먼트입니다.
UCLASS()
class KERNEL_API UKernelItemFragment_Equippable : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	// 이 아이템을 장착했을 때 손에 스폰할 '실체(EquipmentInstance)'의 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UKernelEquipmentInstance> EquipmentClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TObjectPtr<UKernelAbilitySet> GrantAbilitySet;
};