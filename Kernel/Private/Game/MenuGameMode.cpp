// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MenuGameMode.h"
#include "Blueprint/UserWidget.h"

AMenuGameMode::AMenuGameMode()
{
	DefaultPawnClass = nullptr; // 메뉴에선 폰 불필요
}

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 메뉴 위젯 표시 + 입력 모드 UI 전환
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (MainMenuWidgetClass)
		{
			if (UUserWidget* Menu = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass))
			{
				Menu->AddToViewport();
			}
		}
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}
