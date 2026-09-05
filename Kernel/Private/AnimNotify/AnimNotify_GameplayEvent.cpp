// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_GameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	if (!MeshComp->GetWorld()->IsGameWorld()) return;
	
	UAbilitySystemComponent* ASC = 
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
			
	if (ASC)
	{
		FGameplayEventData Payload;
		Payload.Instigator = MeshComp->GetOwner();
		Payload.Target = MeshComp->GetOwner();
		Payload.EventTag = EventTag;
		
		ASC->HandleGameplayEvent(EventTag, &Payload);
		UE_LOG(LogTemp,Warning,TEXT("Notify Sent"))
	}
}
