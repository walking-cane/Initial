// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMenuGameMode();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
