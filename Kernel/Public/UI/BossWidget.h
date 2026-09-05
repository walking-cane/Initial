#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "BossWidget.generated.h"

struct FKernelVerbMessage;
class UProgressBar;
class UWidgetAnimation;
class UKernelHealthComponent; // 디렉터님의 체력 컴포넌트 클래스

UCLASS()
class KERNEL_API UBossWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// GMS는 오직 '전투 돌입/해제' 사건을 들을 때만 씁니다.
	void OnBossStateChanged(
		FGameplayTag Channel, 
		const FKernelVerbMessage& Message);

	// [변경] Lyra 스타일의 체력 델리게이트 수신 콜백 함수
	UFUNCTION()
	void OnHealthChanged(UKernelHealthComponent* HealthComp, float OldValue, float NewValue, AActor* Instigator);

	void BindToBoss(AActor* InBossActor);
	void UnbindFromCurrentBoss();

	UFUNCTION()
	void OnOutroFinished();

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* IntroAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OutroAnimation;

private:
	// 현재 추적 중인 보스의 체력 컴포넌트 포인터 캐싱
	UPROPERTY()
	UKernelHealthComponent* CurrentHealthComp = nullptr;

	FGameplayMessageListenerHandle StateListenerHandle;
};