// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KernelRunStatsSubsystem.generated.h"

struct FGameplayMessageListenerHandle;
struct FGameplayTag;
struct FKernelVerbMessage;
/**
 * 
 */
UCLASS()
class KERNEL_API UKernelRunStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void ResetRun();

private:
	FGameplayMessageListenerHandle StatsUpdateListenerHandle;
	void OnStatsUpdate(FGameplayTag Channel, const FKernelVerbMessage& Message);

	UPROPERTY()
	TMap<FUniqueNetIdRepl, float> TotalDamageByPlayer;
};
