#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "EnemyHealthBarWidget.generated.h"

class UProgressBar;
class UKernelHealthComponent;

UCLASS()
class KERNEL_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindToEnemy(UKernelHealthComponent* InHealthComp);

protected:
	virtual void NativeDestruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnHealthChanged(UKernelHealthComponent* HealthComp, float OldValue, float NewValue, AActor* Instigator);

	void RefreshBar(float Percent);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	// "풀피일 땐 숨기기" 최적화용 — 루트 표시 토글 대상 (BP에서 지정, 선택)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> RootContainer;

private:
	UPROPERTY()
	TObjectPtr<UKernelHealthComponent> CurrentHealthComp;
	
	FGameplayMessageListenerHandle StateListenerHandle;
};