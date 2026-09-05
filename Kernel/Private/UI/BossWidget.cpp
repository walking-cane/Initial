
#include "UI/BossWidget.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "KernelCharacter/KernelHealthComponent.h"
#include "Messages/KernelVerbMessage.h"
#include "Components/ProgressBar.h"

void UBossWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetVisibility(ESlateVisibility::Collapsed);

    FWidgetAnimationDynamicEvent OutroEvent;
    OutroEvent.BindDynamic(this, &UBossWidget::OnOutroFinished);
    BindToAnimationFinished(OutroAnimation, OutroEvent);

    // 보스 방이 열리거나 어그로가 끌리는 전역 상태만 GMS로 구독
    StateListenerHandle = 
        UGameplayMessageSubsystem::Get(this).RegisterListener(
            TAG_Kernel_EnemyState_Changed, 
            this, 
            &UBossWidget::OnBossStateChanged
        );
}

void UBossWidget::NativeDestruct()
{
    UGameplayMessageSubsystem::Get(this).UnregisterListener(StateListenerHandle);
        
    UnbindFromCurrentBoss();
    Super::NativeDestruct();
}

void UBossWidget::OnBossStateChanged(FGameplayTag Channel, const FKernelVerbMessage& Message)
{
    UE_LOG(LogTemp,Warning,TEXT("[BossWidget] OnBossStateChanged"))
    
    if (Message.Tags.HasTagExact(FGameplayTag::RequestGameplayTag("State.Combat")))
    {
        SetVisibility(ESlateVisibility::Visible);
        PlayAnimation(IntroAnimation);
        
        // 메시지에 담겨온 보스 액터 본체를 이용해 델리게이트 바인딩
        if (AActor* BossActor = Cast<AActor>(Message.Instigator))
        {
            BindToBoss(BossActor);
        }
    }
    else if (Message.Tags.HasTagExact(FGameplayTag::RequestGameplayTag("State.Idle")))
    {
        PlayAnimation(OutroAnimation);
        UnbindFromCurrentBoss();
    }
}

void UBossWidget::BindToBoss(AActor* InBossActor)
{
    UnbindFromCurrentBoss();
    if (!InBossActor) return;

    // 보스에서 중계기(HealthComponent)를 찾습니다.
    CurrentHealthComp = InBossActor->FindComponentByClass<UKernelHealthComponent>();
        
    if (CurrentHealthComp)
    {
        FString PlayerName = GetOwningPlayer()->HasAuthority() ? "Server" : "Client";
        UE_LOG(LogTemp,Warning,TEXT("[BossWidget] Binding on %s"), *PlayerName)
        
        // 중계기의 깔끔한 델리게이트에 바인딩합니다.
        CurrentHealthComp->OnHealthChanged.AddDynamic(this, &UBossWidget::OnHealthChanged);

        // 초기 체력 세팅 (GetHealthPercent() 함수 덕분에 계산식도 필요 없습니다!)
        HealthProgressBar->SetPercent(CurrentHealthComp->GetHealthPercent());
    }
}

void UBossWidget::UnbindFromCurrentBoss()
{
    if (CurrentHealthComp)
    {
        CurrentHealthComp->OnHealthChanged.RemoveDynamic(this, &UBossWidget::OnHealthChanged);
        CurrentHealthComp = nullptr;
    }
}

// [변경] 보스의 체력이 깎일 때 전역 필터링 없이 오직 '이 함수만' 정확하게 호출됩니다.
void UBossWidget::OnHealthChanged(
    UKernelHealthComponent* HealthComp, 
    float OldValue, 
    float NewValue, 
    AActor* Instigator)
{
    if (HealthProgressBar && HealthComp)
    {
        HealthProgressBar->SetPercent(HealthComp->GetHealthPercent());
    }
}

void UBossWidget::OnOutroFinished()
{
    SetVisibility(ESlateVisibility::Collapsed);
}