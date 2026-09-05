#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelQuickBarComponent.generated.h"

class UKernelItemInstance;
class UKernelEquipmentInstance;
class UKernelItemManager;

USTRUCT(BlueprintType)
struct FKernelQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	// 변경: WeaponInstance -> ItemInstance (순수 데이터)
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UKernelItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct FKernelQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UKernelItemInstance> ActiveItem;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KERNEL_API UKernelQuickBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKernelQuickBarComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetActiveSlotIndex(int32 NewIndex);
	
	bool AddItemToFreeSlot(UKernelItemInstance* ItemInst);

	UFUNCTION(BlueprintCallable)
	UKernelEquipmentInstance* GetEquippedItem() const { return EquippedItem; }
	TArray<TObjectPtr<UKernelItemInstance>> GetSlots() const { return Slots; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickBar")
	bool HasFreeSlot() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickBar")
	int32 NumSlots = 3;
	
	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<TObjectPtr<UKernelItemInstance>> Slots;
	
	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = 0;
	
	// [핵심] 기존 CurrentInstance를 대체하는 장비 실체 (리플리케이션 필수)
	UPROPERTY(Replicated)
	TObjectPtr<UKernelEquipmentInstance> EquippedItem;
	
	UKernelItemManager* FindItemManager() const;

	void EquipItemInSlot();
	void UnequipItemInSlot();

	UFUNCTION() void OnRep_Slots();
	UFUNCTION() void OnRep_ActiveSlotIndex();
};