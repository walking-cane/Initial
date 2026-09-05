#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KernelInventoryWidget.generated.h"

struct FKernelVerbMessage;
class UHorizontalBox;
class UKernelItemInstance;
class UWeaponSlotEntryWidget;
class UWeaponTooltipWidget;
class UWidgetSwitcher;

UCLASS()
class KERNEL_API UKernelInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void OpenInventory();
	void CloseInventory();
	void ToggleInventory(FGameplayTag Channel, const FKernelVerbMessage& Message);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 무기 칸이 나열되는 곳 */
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UHorizontalBox> WeaponEntryBox;

	/** 무기 / 아티팩트 / 설정 탭 */
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UWidgetSwitcher> TabSwitcher;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UWeaponSlotEntryWidget> WeaponEntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UWeaponTooltipWidget> TooltipClass;

private:
	void RebuildWeaponTab();
	void EnsureTooltip();

	void HandleWeaponSelected(UKernelItemInstance* Weapon, int32 SlotIndex);
	void HandleEntryHoverChanged(UKernelItemInstance* Weapon, bool bHovered);

	UPROPERTY() TObjectPtr<UWeaponTooltipWidget> Tooltip = nullptr;

	bool bIsOpen = false;
	
	FGameplayMessageListenerHandle InventoryListenerHandle;
};