#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelItemManager.generated.h"

class UKernelItemInstance;
class UKernelEquipmentInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelItemManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKernelItemManager();

	// 아이템을 가방(Inventory)에 넣습니다.
	bool AddItemToInventory(UKernelItemInstance* NewInstance);

	// 가방에 있는 아이템을 손에 쥡니다. (EquipmentInstance 스폰)
	UKernelEquipmentInstance* EquipItem(UKernelItemInstance* ItemInstance);
	
	// 손에 쥐고 있는 장비를 해제합니다. (EquipmentInstance 파괴)
	void UnEquipItem(UKernelItemInstance* ItemInstance);
	
	// 아이템을 바닥에 버립니다.
	void DropItem(UKernelItemInstance* DropInstance);
	
	// 무기인지 체크
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanAcceptItem(const UKernelItemInstance* Inst) const;

protected:
	// 1. 가방 목록 (순수 데이터)
	UPROPERTY(Replicated)	
	TArray<TObjectPtr<UKernelItemInstance>> InventoryList;

	// 2. 현재 장착된 장비 목록 (물리적 실체)
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UKernelEquipmentInstance>> EquipmentList;
};