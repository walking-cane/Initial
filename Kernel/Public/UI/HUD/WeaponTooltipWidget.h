#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponTooltipWidget.generated.h"

class UAffixEntryWidget;
class UImage;
class UKernelItemInstance;
class UTextBlock;
class UVerticalBox;

UCLASS()
class KERNEL_API UWeaponTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowFor(UKernelItemInstance* InWeapon);
	void HideTooltip();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(meta = (BindWidget))         TObjectPtr<UTextBlock>   WeaponName;
	UPROPERTY(meta = (BindWidget))         TObjectPtr<UVerticalBox> AffixListBox;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UImage>       RarityBorder;

	/** 접사 한 줄 위젯 — WBP_AffixEntry */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UAffixEntryWidget> AffixEntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector2D CursorOffset = FVector2D(20.f, 20.f);

private:
	void UpdatePosition();
};