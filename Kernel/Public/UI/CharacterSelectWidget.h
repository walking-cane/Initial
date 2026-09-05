// CharacterSelectWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CharacterSelectWidget.generated.h"

class UVerticalBox;
class UKernelCharacterPickRow;
class UImage;
class UTextBlock;
class AKernelGameState;
class AKernelPlayerState;
class UKernelCharacterSlot;
class UPanelWidget;
class UButton;

UCLASS()
class KERNEL_API UCharacterSelectWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	void BuildSlots();            // 로스터 → 슬롯 생성 (해금 정보 도착 시 재생성)
	void BindToPlayerStates();    // 늦은 접속자/늦은 복제 대응 폴링
	void RequestCharacter(int32 Index);
	void RefreshCharacterPicks();
	void RefreshStartButton();
	void RefreshPlayerList();

	UFUNCTION() void OnStartButtonClicked();

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> PlayerListBox;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UPanelWidget> CharacterListBox;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> StartButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> PickedCharName;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> PickedImage;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient) TObjectPtr<UWidgetAnimation> PickedCharNameAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<UKernelCharacterSlot> CharacterSlotClass;
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<UKernelCharacterPickRow> PlayerRowClass;

private:
	UPROPERTY() TArray<TObjectPtr<UKernelCharacterSlot>> CharacterSlots;
	UPROPERTY() TObjectPtr<AKernelGameState> CachedGameState;
	UPROPERTY() TArray<TObjectPtr<UKernelCharacterPickRow>> PlayerRows;

	TSet<TWeakObjectPtr<AKernelPlayerState>> BoundPlayerStates;
	FTimerHandle PlayerPollHandle;
};