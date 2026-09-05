// KernelPlayerPickRow.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KernelCharacterPickRow.generated.h"

class UImage;
class UTextBlock;
class UKernelCharacterDefinition;

UCLASS()
class KERNEL_API UKernelCharacterPickRow : public UUserWidget
{
	GENERATED_BODY()

public:
	/** @param Def   고른 캐릭터 (null이면 아직 미선택)
	 *  @param bIsMe 내 줄인지 — 강조 표시용 */
	void UpdateRow(const FText& PlayerName, const UKernelCharacterDefinition* Def, bool bIsMe);

protected:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> PlayerNameText;

	/** 초상화가 들어갈 네모칸 */
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> PortraitImage;

	/** 미선택 상태 표시 (물음표·빈 아이콘 등, 선택) */
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UImage> EmptyIcon;

	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UImage> MyHighlight;
};