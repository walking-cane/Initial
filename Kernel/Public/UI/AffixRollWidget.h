#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/KernelItemTypes.h"
#include "AffixRollWidget.generated.h"

class UAffixEntryWidget;
class UWeaponSlotEntryWidget;
class UButton;
class UHorizontalBox;
class UKernelItemInstance;
class UVerticalBox;
struct FGameplayTag;
struct FKernelInteractionMessage;

UCLASS()
class KERNEL_API UAffixRollWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 소유 무기 칸이 나열되는 곳 */
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UHorizontalBox> WeaponListBox;

	/** 선택한 무기의 접사 줄이 나열되는 곳 */
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> AffixListBox;

	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UButton> CloseButton;

private:
	void OnOpen(FGameplayTag Channel, const FKernelInteractionMessage& Message);

	UFUNCTION()
	void CloseWindow();

	void BuildWeaponList();
	void BuildAffixRows();

	void HandleWeaponSelected(UKernelItemInstance* InWeapon, int32 InSlotIndex);
	void HandleAffixRerollRequested(int32 AffixIndex, FName AffixId);
	void HandleAffixRerolled(int32 InSlotIndex, int32 AffixIndex, const FKernelAppliedAffix& NewApplied);
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> BuildAnimation;

	/** 무기 칸 위젯 — WBP_AffixRollEntry */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UWeaponSlotEntryWidget> WeaponEntryClass;

	/** 접사 줄 위젯 — WBP_AffixEntry */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UAffixEntryWidget> AffixEntryClass;

	FGameplayMessageListenerHandle RollAffixListenerHandle;

	TWeakObjectPtr<UKernelItemInstance> SelectedWeapon;
	int32 SelectedSlotIndex = INDEX_NONE;
};