// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KernelCharacter/KernelCharacterBase.h"
#include "Item/KernelItemTypes.h"
#include "Cosmetics/KernelCosmeticInterface.h"
#include "KernelEnemyCharacter.generated.h"

class UTimelineComponent;
class UKernelHealthComponent;
class UKernelCombatAttributeSet;
class UKernelHealthAttributeSet;
class UWidgetComponent;

UCLASS()
class KERNEL_API AKernelEnemyCharacter : public AKernelCharacterBase, public IKernelCosmeticInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "AttributeSet") TObjectPtr<UKernelCombatAttributeSet> CombatSet;
	UPROPERTY(VisibleAnywhere, Category = "AttributeSet") TObjectPtr<UKernelHealthAttributeSet> HealthSet;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UWidgetComponent> HealthBarComponent;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UKernelAbilitySystemComponent> KernelASC;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	float DropChance = 0.3f; // 플레이어당 독립 확률

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	TArray<TObjectPtr<UKernelItemDefinition>> DropTable;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	TMap<EItemRarity, int32> RarityWeights;    
	
	void InitializeASC();
	
	UPROPERTY(VisibleAnywhere, Category = "Dissolve")
	TObjectPtr<UTimelineComponent> DissolveTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Dissolve")
	TObjectPtr<UCurveFloat> DissolveCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Dissolve")
	FName DissolveParamName = FName("DissolveAmount");
	
public:
	AKernelEnemyCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	virtual UAnimMontage* GetDeathMontage() override;
	virtual void DropItemOnDeath();
	
	void HandleDeathStarted();
	
	void TryInitHealthBar();
	void StartDissolve(float Duration, bool bOut);
	void ResetDissolve();
	
private:
	FTimerHandle RetryBindHandle;
	
	UFUNCTION() void HandleDissolveProgress(float Value);
	
	void EnsureDissolveMIDs();

	UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> DissolveMIDs;
};
