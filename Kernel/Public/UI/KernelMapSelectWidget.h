// KernelLobbyWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "KernelMapSelectWidget.generated.h"

class UImage;
class UButton;
class UCommonButtonBase;
class UTextBlock;
class UPanelWidget;
class AKernelGameState;
class AKernelPlayerState;
class UKernelMapSlotWidget;

UCLASS()
class KERNEL_API UKernelMapSelectWidget : public UCommonActivatableWidget 
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	void OnMapSlotClicked(int32 MapIndex);
	UFUNCTION()
	void OnStartClicked();

	void RefreshVotes();                  // 전원 투표 표시 갱신
	void RefreshHostSelection();          // 호스트 선택 하이라이트 + START 활성화
	void HandleMapPing(const FString& PlayerName, int32 MapIndex); // 요구 3: 핑 채팅
	void BindToPlayerStates();            // 늦은 접속자 폴링 — 기존 패턴 유지

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UPanelWidget> MapListBox;   // WrapBox/HorizontalBox 등
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> StartButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> PingChatText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> MapImage;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<UKernelMapSlotWidget> MapSlotClass;

private:
	UPROPERTY() TObjectPtr<AKernelGameState> CachedGameState;
	UPROPERTY() TArray<TObjectPtr<UKernelMapSlotWidget>> MapSlots;

	TSet<TWeakObjectPtr<AKernelPlayerState>> BoundPlayerStates;
	FTimerHandle PlayerPollHandle;
	TArray<FString> PingLines; // 최근 핑 몇 줄
};