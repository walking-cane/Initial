// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataTable.h"
#include "KernelAffixDistributionRow.generated.h"

class UKernelAffixDefinition;

USTRUCT(BlueprintType)
struct FKernelAffixDistributionRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Affix")
	TSoftObjectPtr<UKernelAffixDefinition> AffixDef;

	/** 비우면 공용 Affix, 설정하면 설정한 무기에서만 등장 */
	UPROPERTY(EditAnywhere, Category="Scope", meta=(Categories="Weapon"))
	FGameplayTagContainer AllowedWeapons;

	/** true 면 단 하나만 나옴. */
	UPROPERTY(EditAnywhere, Category="Scope")
	bool bExclusive = false;

	/** 이 Affix가 등장하는 최소 Rarity */
	UPROPERTY(EditAnywhere, Category="Rarity", meta=(Categories="Rarity"))
	FGameplayTag MinRarity;

	UPROPERTY(EditAnywhere, Category="Roll")
	int32 Weight = 100;

	// 핵심: 레어리티별 수치는 행을 복제하지 말고 커브로
	UPROPERTY(EditAnywhere, Category="Roll")
	FScalableFloat Magnitude;
};
