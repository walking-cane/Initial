// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Artifact/KernelArtifactDefinition.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "KernelArtifactEntryWidget.generated.h"

class UButton;
class UTextBlock;
class UKernelArtifactDefinition;
struct FKernelArtifactOfferMessage;
class UHorizontalBox;
class UImage;
class UVerticalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickArtifactEntry, UKernelArtifactEntryWidget*);

UCLASS()
class KERNEL_API UKernelArtifactEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetEntry(const UKernelArtifactDefinition* Def, int32 InOfferId, int32 InChoiceId);
	
	FOnClickArtifactEntry OnClickArtifactEntry;
	
	int32 EntryIndex = INDEX_NONE;
	
	void OnPicked();
	void OnUnPicked();
	
	void SetSelectEnabled(bool bEnabled) { SelectButton->SetIsEnabled(bEnabled); }
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Description;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ArtifactImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> FeatureTextBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> PickedAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> UnPickedAnim;
	
	FText GetArtifactTypeText(EKernelArtifactType Type)
	{
		return StaticEnum<EKernelArtifactType>()->GetDisplayNameTextByValue(static_cast<int64>(Type));
	}
	
private:
	UFUNCTION() void OnSelectClicked();
	
	int32 OfferId  = INDEX_NONE;
	int32 ChoiceId = INDEX_NONE;
};
