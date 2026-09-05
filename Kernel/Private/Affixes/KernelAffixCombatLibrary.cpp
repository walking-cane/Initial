#include "Affixes/KernelAffixCombatLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Affixes/KernelAffixDefinition.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemInstance.h"

void UKernelAffixCombatLibrary::ApplyAffixOnHit(
	UAbilitySystemComponent* InstigatorASC, 
	UAbilitySystemComponent* TargetASC,
	UKernelItemInstance* SourceWeapon, 
	const FHitResult& Hit)
{
	if (!InstigatorASC || !SourceWeapon || !TargetASC)
	{
		UE_LOG(LogTemp,Error,TEXT("[AffixEffect] No Data"));
		return;
	}
	
	if (TargetASC->HasMatchingGameplayTag(TAG_Status_Death_Dying)) return;
	
	if (SourceWeapon->GetAffixes().IsEmpty())
	{
		UE_LOG(LogTemp,Error,TEXT("[AffixEffect] No Affixes"));
		return;
	}
	
	if (!InstigatorASC->GetOwner() || !InstigatorASC->GetOwner()->HasAuthority()) return;
	
	FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
	Context.AddSourceObject(SourceWeapon);
	Context.AddHitResult(Hit); // 부위 판정, 임팩트 위치 등에 필요
	
	for (const FKernelAppliedAffix& Applied : SourceWeapon->GetAffixes())
	{
		const UKernelAffixDefinition* AffixDef = Applied.Affix;
		if (!AffixDef || !AffixDef->OnHitEffectToTarget) continue;
		
		if (AffixDef->ApplyType == EAffixApplyType::Chance)
		{
			if (FMath::FRand() > Applied.RolledChance) continue; // Chance 타입 Affix가 확률에 떨어지면 continue.
		}

		// 대상에게
		if (AffixDef->OnHitEffectToTarget)
		{
			FGameplayEffectSpecHandle Spec =
				InstigatorASC->MakeOutgoingSpec(AffixDef->OnHitEffectToTarget, 1.f, Context);
			if (Spec.IsValid())
			{
				FActiveGameplayEffectHandle Handle =
					InstigatorASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
				UE_LOG(LogTemp, Log, TEXT("[AffixEffect] Handle=%s, Duration=%.2f, Period=%.2f"),
					Handle.IsValid() ? TEXT("Valid") : TEXT("INVALID"),
					Spec.Data->GetDuration(), Spec.Data->GetPeriod());
			}
		}

		// 자신에게 (흡혈 등)
		if (AffixDef->OnHitEffectToSelf)
		{
			FGameplayEffectSpecHandle Spec = 
				InstigatorASC->MakeOutgoingSpec(AffixDef->OnHitEffectToSelf, 1.f, Context);
			if (Spec.IsValid())
			{
				InstigatorASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}

		// 이벤트 트리거 (연속 공격 등)
		if (AffixDef->TriggerEventTag.IsValid())
		{
			FGameplayEventData EventData;
			EventData.EventTag = AffixDef->TriggerEventTag;
			EventData.Instigator = InstigatorASC->GetOwner();
			EventData.Target = TargetASC->GetAvatarActor();
			EventData.ContextHandle = Context;
			InstigatorASC->HandleGameplayEvent(AffixDef->TriggerEventTag, &EventData);
		}
	}
}
