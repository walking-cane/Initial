#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"   // [추가] 값 타입이라 전방선언으론 부족
#include "GameplayAbilitySpecHandle.h"    // [추가] 동일
#include "AttributeSet.h"                 // [추가] TObjectPtr<UAttributeSet> 배열용
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "KernelAbilitySet.generated.h"

class UKernelAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FKernelAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct FKernelAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FKernelAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UKernelAbilitySystemComponent* KernelASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

UCLASS(BlueprintType, Const)
class KERNEL_API UKernelAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	void GiveToAbilitySystem(
		UKernelAbilitySystemComponent* KernelASC,
		FKernelAbilitySet_GrantedHandles* OutGrantedHandles,
		UObject* SourceObject = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FKernelAbilitySet_GameplayAbility> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FKernelAbilitySet_GameplayEffect> GrantedEffects;

	// 어트리뷰트셋 부여는 아직 미사용 — 나중에 켤 때 TakeFromAbilitySystem은 이미 대응돼 있음
	// UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets")
	// TArray<FKernelAbilitySet_AttributeSet> GrantedAttributes;
};