// Fill out your copyright notice in the Description page of Project Settings.


#include "Artifact/KernelArtifactComponent.h"

#include "AbilitySystemGlobals.h"
#include "Artifact/KernelArtifactDefinition.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

void UKernelArtifactComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKernelArtifactComponent, Entries);
}

// Sets default values for this component's properties
UKernelArtifactComponent::UKernelArtifactComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKernelArtifactComponent::GrantArtifact(const UKernelArtifactDefinition* Def)
{
	if (!Def) return;
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	UKernelAbilitySystemComponent* ASC = Cast<UKernelAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	if (!ASC) return;

	FKernelArtifactEntry* Existing = Entries.FindByPredicate(
		[Def](const FKernelArtifactEntry& E) { return E.Definition == Def; });

	if (Existing && Def->bUnique)
	{
		return;   // 중복 불가인데 이미 보유
	}

	if (Existing)
	{
		// 스택: 소유한 아티팩트라면 중첩 부여
		if (Def->GrantSet)
		{
			Def->GrantSet->GiveToAbilitySystem(ASC, &Existing->GrantedHandles, this);
		}
		++Existing->StackCount;
	}
	else
	{
		FKernelArtifactEntry NewEntry;
		NewEntry.Definition = Def;
		NewEntry.StackCount = 1;

		if (Def->GrantSet)
		{
			Def->GrantSet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, this);
		}
		Entries.Add(MoveTemp(NewEntry));
	}

	OnRep_Entries();
}

void UKernelArtifactComponent::RevokeArtifact(const UKernelArtifactDefinition* Def)
{
	if (!IsValid(Def)) return;
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	UKernelAbilitySystemComponent* ASC = Cast<UKernelAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	if (!ASC) return;

	for (int32 i = Entries.Num() - 1; i >= 0; --i)
	{
		if (Entries[i].Definition != Def) continue;

		Entries[i].GrantedHandles.TakeFromAbilitySystem(ASC);
		UE_LOG(LogTemp, Log, TEXT("[ArtifactComponent] Removed: %s"),
			*GetNameSafe(Entries[i].Definition));

		Entries.RemoveAt(i);
	}

	OnRep_Entries();
}

bool UKernelArtifactComponent::HasArtifact(const UKernelArtifactDefinition* Def) const
{
	for (const FKernelArtifactEntry& Entry : Entries)
	{
		if (Entry.Definition == Def)
		{
			return true;
		}
	}
	
	return false;
}

int32 UKernelArtifactComponent::GetStackCount(const UKernelArtifactDefinition* Def) const
{
	for (const FKernelArtifactEntry& Entry : Entries)
	{
		if (Entry.Definition == Def)
		{
			return Entry.StackCount;
		}
	}
	return 0;
}

void UKernelArtifactComponent::OnRep_Entries()
{
	// TODO : Visual GMS.
}
