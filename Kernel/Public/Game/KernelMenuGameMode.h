// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KernelMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API AKernelMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AKernelMenuGameMode();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
