// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KernelMainMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CommonButtonBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/KernelMenuFlowSubsystem.h"

void UKernelMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (UCommonButtonBase* Button : {StartButton.Get(), JoinButton.Get(), QuitButton.Get() })
	{
		if (Button)
		{
			Button->OnClicked().AddUObject(this, &UKernelMainMenuWidget::OnMenuButtonClicked, Button);
			Button->OnHovered().AddUObject(this, &UKernelMainMenuWidget::OnMenuButtonHovered, Button);
		}
	}
	
	if (SelectFadeAnimation)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &UKernelMainMenuWidget::OnFadeOutFinished);
		BindToAnimationFinished(SelectFadeAnimation, FinishedEvent);
	}
	
	CachedAudioComp = UGameplayStatics::SpawnSound2D(this, MenuTheme);
}

void UKernelMainMenuWidget::NativeDestruct()
{
	for (UCommonButtonBase* Button : { StartButton.Get(), JoinButton.Get(), QuitButton.Get() })
	{
		if (Button)
		{
			Button->OnClicked().RemoveAll(this);
			Button->OnHovered().RemoveAll(this);
		}
	}
	Super::NativeDestruct();
}

void UKernelMainMenuWidget::OnMenuButtonClicked(UCommonButtonBase* Button)
{
	if (PendingAction != EMenuAction::None)
	{
		return;
	}
	
	if (Button == StartButton) { PendingAction = EMenuAction::Start; }
	else if (Button == JoinButton) { PendingAction = EMenuAction::Join; }
	else if (Button == QuitButton) { PendingAction = EMenuAction::Quit; }
	else { return; }

	// 2. 페이드 중 추가 입력 차단
	SetIsEnabled(false);

	// 3. 페이드 시작 — 실행은 OnFadeOutFinished에서
	if (SelectFadeAnimation && CachedAudioComp)
	{
		PlayAnimation(SelectFadeAnimation);
		CachedAudioComp->FadeOut(2.f, 0.f);
	}
	else
	{
		OnFadeOutFinished(); // 애니메이션 미할당 시에도 기능은 동작하게 폴백
	}
}

void UKernelMainMenuWidget::OnMenuButtonHovered(UCommonButtonBase* Button)
{
	UE_LOG(LogTemp, Verbose, TEXT("[MainMenu] Hovered: %s"), *GetNameSafe(Button));
}

void UKernelMainMenuWidget::HandleStart()
{
	if (UKernelMenuFlowSubsystem* Flow = GetGameInstance()->GetSubsystem<UKernelMenuFlowSubsystem>())
	{
		Flow->HostGame(); // 바로 listen 트래블 → 게임 맵 로비에서 선택
	}
}

void UKernelMainMenuWidget::HandleJoin()
{
	if (UKernelMenuFlowSubsystem* Flow = GetGameInstance()->GetSubsystem<UKernelMenuFlowSubsystem>())
	{
		Flow->FindGames(); // 지금은 로그만 — 4단계에서 서버 리스트 화면과 함께 구현
	}
}

void UKernelMainMenuWidget::HandleQuit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UKernelMainMenuWidget::OnFadeOutFinished()
{
	const EMenuAction Action = PendingAction;
	PendingAction = EMenuAction::None;

	switch (Action)
	{
	case EMenuAction::Start: HandleStart(); break;
	case EMenuAction::Join: HandleJoin(); break;
	case EMenuAction::Quit: HandleQuit(); break;
	default: break;
	}
}