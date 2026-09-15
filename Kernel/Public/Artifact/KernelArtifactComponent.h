// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbility/AbilitySets/KernelAbilitySet.h"
#include "KernelArtifactComponent.generated.h"

class UKernelArtifactDefinition;

USTRUCT()
struct FKernelArtifactEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<const UKernelArtifactDefinition> Definition;

	UPROPERTY()
	int32 StackCount = 0;

	// 복제 금지 — 서버 로컬 핸들 
	FKernelAbilitySet_GrantedHandles GrantedHandles;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KERNEL_API UKernelArtifactComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UKernelArtifactComponent();

	/** 서버 전용. 획득 경로는 GameMode → PlayerState */
	void GrantArtifact(const UKernelArtifactDefinition* Def);

	/** 저주 해제 등 제거가 필요한 경우에만 */
	void RevokeArtifact(const UKernelArtifactDefinition* Def);

	bool HasArtifact(const UKernelArtifactDefinition* Def) const;
	int32 GetStackCount(const UKernelArtifactDefinition* Def) const;

	TArray<FKernelArtifactEntry> GetEntries() const { return Entries; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Entries)
	TArray<FKernelArtifactEntry> Entries;

	UFUNCTION() void OnRep_Entries();
};
