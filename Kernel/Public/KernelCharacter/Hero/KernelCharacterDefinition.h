// KernelCharacterDefinition.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/KernelItemTypes.h"
#include "KernelCharacterDefinition.generated.h"

class UKernelAbilitySet;

UCLASS(BlueprintType)
class KERNEL_API UKernelCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName CharacterId;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> Portrait;
	
	/** 게임 시작 시 지급할 무기들 */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<UKernelItemDefinition> StartingItem;
	
	/** 시작 무기 등급 — 보통 Normal 고정 */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	EItemRarity StartingItemRarity = EItemRarity::Starting;

	// 메뉴 프리뷰
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
	TSoftObjectPtr<USkeletalMesh> PreviewMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
	TSubclassOf<UAnimInstance> PreviewAnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<UKernelAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	TSoftObjectPtr<USkeletalMesh> Mesh3P;
	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	TSoftObjectPtr<USkeletalMesh> Mesh1P;
	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	TSubclassOf<UAnimInstance> AnimClass3P;
	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	TSubclassOf<UAnimInstance> AnimClass1P;
};