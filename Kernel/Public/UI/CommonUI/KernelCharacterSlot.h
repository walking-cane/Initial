// CharacterSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "KernelCharacterSlot.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UKernelCharacterDefinition;

DECLARE_DELEGATE_OneParam(FOnCharacterSlotClicked, int32 /*CharacterIndex*/);

UCLASS()
class KERNEL_API UKernelCharacterSlot : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** bUnlocked는 GameState가 아니라 '소유 PlayerState'에서 판정해 넘겨받는다 — 맵 슬롯과 다른 유일한 지점 */
	void InitSlot(int32 InIndex, const UKernelCharacterDefinition* Def, bool bUnlocked);
	
	/** 내가 지금 고른 캐릭터인지 하이라이트 */
	void SetMyPick(bool bMine);

	int32 GetCharacterIndex() const { return CharacterIndex; }
	FOnCharacterSlotClicked OnSlotClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION() void HandleClicked();
	UFUNCTION() void HandleHovered();
	UFUNCTION() void HandleUnhovered();

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>    SlotButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage>     PortraitImage;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage>     LockIcon;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage>     SelectedBorder;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient) TObjectPtr<UWidgetAnimation> HoveredAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient) TObjectPtr<UWidgetAnimation> PickedAnim;

private:
	int32 CharacterIndex = INDEX_NONE;
	bool bIsUnlocked = false;
};