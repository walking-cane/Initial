#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KernelHUD.generated.h"

class AKernelHeroCharacter;
enum class EKernelGamePhase : uint8;
class UBossWidget;

UCLASS()
class KERNEL_API AKernelHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> GameplayLayoutClass; // W_GameplayLayout
	
	// 블루프린트에서 'WBP_BossWidget'을 연결해 줄 클래스 변수
	UPROPERTY(EditDefaultsOnly, Category = "Kernel|UI")
	TSubclassOf<UBossWidget> BossWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Kernel|UI")
	TSubclassOf<UUserWidget> MapSelectWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Kernel|UI")
	TSubclassOf<UUserWidget> CharacterSelectWidgetClass;
	
	void TryBindToGameState(); // GameState 복제 지연 대응
	void HandleGamePhase(EKernelGamePhase NewPhase);
	void ShowWidget(TObjectPtr<UUserWidget>& Instance, TSubclassOf<UUserWidget> Class);
	void HideWidget(TObjectPtr<UUserWidget>& Instance);

private:
	// 생성된 위젯을 들고 있을 포인터 (가비지 컬렉션 방지)
	UPROPERTY() TObjectPtr<UUserWidget> BossWidgetInstance = nullptr;
	UPROPERTY() TObjectPtr<UUserWidget> MapSelectWidgetInstance = nullptr;
	UPROPERTY() TObjectPtr<UUserWidget> CharacterSelectWidgetInstance = nullptr;
	
	UPROPERTY() TObjectPtr<UUserWidget> GameplayLayout = nullptr;
};