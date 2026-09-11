// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"
#include "KernelEnemy_Flying.generated.h"

/**
 * 
 */
UCLASS()
class KERNEL_API AKernelEnemy_Flying : public AKernelEnemyCharacter
{
	GENERATED_BODY()
	
public:
	AKernelEnemy_Flying(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	void MaintainHoverHeight(float DeltaTime);
	
	UPROPERTY(EditAnywhere, Category = "Hover")
	float HoverHeight = 500.f;
	UPROPERTY(EditAnywhere, Category = "Hover")
	float HoverHeightOffset = 50.f;
	UPROPERTY(EditAnywhere, Category = "Hover")
	float HoverInterpSpeed = 1.f;
};
