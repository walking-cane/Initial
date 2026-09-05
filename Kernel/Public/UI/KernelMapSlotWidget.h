// KernelMapSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "KernelMapSlotWidget.generated.h"

class UButton;
class UCommonButtonBase;
class UImage;
class UTextBlock;
class UKernelMapDefinition;

DECLARE_DELEGATE_OneParam(FOnMapSlotClicked, int32 /*MapIndex*/);

UCLASS()
class KERNEL_API UKernelMapSlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitSlot(int32 InMapIndex, const UKernelMapDefinition* MapDef);
	void SetVoterNames(const FString& Names);      // 요구 4: 맵 위에 투표자 표시

	FOnMapSlotClicked OnSlotClicked;
	int32 GetMapIndex() const { return MapIndex; }

protected:
	virtual void NativeConstruct() override;

	UFUNCTION() void HandleClicked();
	UFUNCTION() void HandleHovered();
	UFUNCTION() void HandleUnhovered();

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> SlotButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> ThumbnailImage;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> MapNameText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> LockIcon;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> VoterListText;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient) TObjectPtr<UWidgetAnimation> HoveredAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient) TObjectPtr<UWidgetAnimation> ClickedAnim;

private:
	int32 MapIndex = INDEX_NONE;
	bool bLocked = false;
};