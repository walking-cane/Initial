// GCN_DamagePop.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_DamagePop.generated.h"

class UNiagaraSystem;

UCLASS()
class KERNEL_API UGCN_DamagePop : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	UPROPERTY(EditDefaultsOnly, Category = "DamagePop")
	TObjectPtr<UNiagaraSystem> DamageNumberSystemAsset;
	
	// 내가 가한 데미지 색
	UPROPERTY(EditDefaultsOnly, Category = "DamagePop")
	FLinearColor MyDamageColor = FLinearColor(1.f, 0.85f, 0.1f, 1.f);

	// 다른 플레이어가 가한 데미지 색 (반투명 회색)
	UPROPERTY(EditDefaultsOnly, Category = "DamagePop")
	FLinearColor OtherDamageColor = FLinearColor(0.6f, 0.6f, 0.6f, 0.35f);
	
	/** DamageType 에 따른 컬러 Map */
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TMap<FGameplayTag, FLinearColor> DamageTypeColors;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FLinearColor DefaultDamageColor = FLinearColor::White;
	
private:
	FLinearColor GetDamageColor(const FGameplayTagContainer& Tags) const;
};