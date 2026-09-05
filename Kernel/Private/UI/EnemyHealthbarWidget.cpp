#include "UI/EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "KernelCharacter/KernelHealthComponent.h"

void UEnemyHealthBarWidget::BindToEnemy(UKernelHealthComponent* InHealthComp)
{
	// 기존 바인딩 정리 (재사용/재바인딩 대비)
	if (CurrentHealthComp)
	{
		CurrentHealthComp->OnHealthChanged.RemoveDynamic(this, &UEnemyHealthBarWidget::OnHealthChanged);
	}

	CurrentHealthComp = InHealthComp;
	if (!CurrentHealthComp) return;

	CurrentHealthComp->OnHealthChanged.AddDynamic(this, &UEnemyHealthBarWidget::OnHealthChanged);

	// 초기 체력 반영
	RefreshBar(CurrentHealthComp->GetHealthPercent());
}

void UEnemyHealthBarWidget::OnHealthChanged(UKernelHealthComponent* HealthComp, float OldValue, float NewValue, AActor* Instigator)
{
	if (HealthComp)
	{
		RefreshBar(HealthComp->GetHealthPercent());
	}
}

void UEnemyHealthBarWidget::RefreshBar(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}

	// "풀피일 땐 숨기기" 최적화: 데미지를 안 받았으면 체력바를 안 보이게
	if (RootContainer)
	{
		const bool bShouldShow = (Percent < 0.999f);
		RootContainer->SetVisibility(bShouldShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UEnemyHealthBarWidget::NativeDestruct()
{
	if (CurrentHealthComp)
	{
		CurrentHealthComp->OnHealthChanged.RemoveDynamic(this, &UEnemyHealthBarWidget::OnHealthChanged);
		CurrentHealthComp = nullptr;
	}
	Super::NativeDestruct();
}

void UEnemyHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (RootContainer)
	{
		RootContainer->SetVisibility(ESlateVisibility::Collapsed);
	}
}
