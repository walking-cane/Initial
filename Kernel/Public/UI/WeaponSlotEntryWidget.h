#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlotEntryWidget.generated.h"

class UButton;
class UImage;
class UKernelItemInstance;
class UWidgetAnimation;

DECLARE_DELEGATE_TwoParams(FOnWeaponSelected,     UKernelItemInstance* /*Weapon*/, int32 /*SlotIndex*/);
DECLARE_DELEGATE_TwoParams(FOnWeaponHoverChanged, UKernelItemInstance* /*Weapon*/, bool  /*bHovered*/);

UCLASS()
class KERNEL_API UWeaponSlotEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** InWeapon이 null이면 빈 슬롯으로 그린다 (인벤토리는 빈 칸도 보여야 함) */
	void SetEntry(UKernelItemInstance* InWeapon, int32 InSlotIndex);

	UKernelItemInstance* GetWeapon() const { return Weapon; }
	int32 GetSlotIndex() const { return SlotIndex; }

	FOnWeaponSelected     OnWeaponSelected;
	FOnWeaponHoverChanged OnWeaponHoverChanged;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage>  WeaponImage;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage>  Border;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	TObjectPtr<UWidgetAnimation> EntryConstructAnim;

	/** 빈 슬롯 테두리 색 */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FLinearColor EmptySlotColor = FLinearColor(0.14f, 0.14f, 0.14f, 1.f);

private:
	UFUNCTION() void HandleClicked();
	UFUNCTION() void HandleHovered();
	UFUNCTION() void HandleUnhovered();

	UPROPERTY() TObjectPtr<UKernelItemInstance> Weapon = nullptr;

	int32 SlotIndex = INDEX_NONE;
};