// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosmetics/KernelCosmeticComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UKernelCosmeticComponent::UKernelCosmeticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UKernelCosmeticComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKernelCosmeticComponent, WeaponAttachEntries);
	DOREPLIFETIME(UKernelCosmeticComponent, CurrentLayer1P);
	DOREPLIFETIME(UKernelCosmeticComponent, CurrentLayer3P);
	DOREPLIFETIME(UKernelCosmeticComponent, EquipMontage);
	DOREPLIFETIME(UKernelCosmeticComponent, EquipMontageCounter);
}

void UKernelCosmeticComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroySpawnedWeaponActors();
	Super::EndPlay(EndPlayReason);
}

// ---------------------------------------------------------------------------
// 무기 부착
// ---------------------------------------------------------------------------

void UKernelCosmeticComponent::SetWeaponAttachEntries(const TArray<FKernelWeaponAttachEntry>& NewEntries)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	WeaponAttachEntries = NewEntries;

	// 서버(리슨 호스트) 본인도 동일 경로를 타야 하므로 직접 호출한다.
	OnRep_WeaponAttachEntries();
}

void UKernelCosmeticComponent::ClearWeaponAttachEntries()
{
	SetWeaponAttachEntries(TArray<FKernelWeaponAttachEntry>());
}

void UKernelCosmeticComponent::OnRep_WeaponAttachEntries()
{
	// 배열이 통째로 갈리므로, 이전 무기를 먼저 지우고 새로 만든다.
	DestroySpawnedWeaponActors();
	SpawnWeaponActors();
}

void UKernelCosmeticComponent::DestroySpawnedWeaponActors()
{
	for (AActor* Actor : SpawnedWeaponActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedWeaponActors.Reset();
}

void UKernelCosmeticComponent::SpawnWeaponActors()
{
	if (WeaponAttachEntries.Num() == 0)
	{
		return;
	}

	IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner());
	if (!CosmeticTarget)
	{
		return;
	}

	ACharacter* Character = GetOwner<ACharacter>();
	const bool bLocallyControlled = Character && Character->IsLocallyControlled();

	USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P();
	USkeletalMeshComponent* Mesh3P = Character ? Character->GetMesh() : nullptr;

	for (const FKernelWeaponAttachEntry& Entry : WeaponAttachEntries)
	{
		if (!Entry.WeaponActorClass)
		{
			continue;
		}

		// 1P는 이 폰을 직접 조종하는 머신에서만 필요하다.
		if (bLocallyControlled && Mesh1P)
		{
			if (AActor* Spawned = SpawnAndAttachWeapon(
				Entry, Mesh1P, Entry.AttachSocket1P,
				/*bOnlyOwnerSee=*/true, /*bOwnerNoSee=*/false))
			{
				SpawnedWeaponActors.Add(Spawned);
			}
		}

		// 3P는 모든 머신에서 만든다. 소유자 화면에서는 그림자만 남는다.
		if (Mesh3P)
		{
			if (AActor* Spawned = SpawnAndAttachWeapon(
				Entry, Mesh3P, Entry.AttachSocket3P,
				/*bOnlyOwnerSee=*/false, /*bOwnerNoSee=*/true))
			{
				SpawnedWeaponActors.Add(Spawned);
			}
		}
	}
}

AActor* UKernelCosmeticComponent::SpawnAndAttachWeapon(
	const FKernelWeaponAttachEntry& Entry,
	USkeletalMeshComponent* AttachTarget,
	FName SocketName,
	bool bOnlyOwnerSee,
	bool bOwnerNoSee)
{
	UWorld* World = GetWorld();
	if (!World || !AttachTarget || !Entry.WeaponActorClass)
	{
		return nullptr;
	}
	
	if (SocketName != NAME_None && !AttachTarget->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Cosmetic] Socket '%s' not found on %s. Weapon would attach to component root."),
			*SocketName.ToString(), *GetNameSafe(AttachTarget));
		return nullptr;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	AActor* NewActor = World->SpawnActorDeferred<AActor>(
		Entry.WeaponActorClass,
		FTransform::Identity,
		GetOwner(),
		OwnerPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!NewActor)
	{
		return nullptr;
	}

	NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/true);

	NewActor->AttachToComponent(
		AttachTarget,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName);

	// 소켓에 스냅한 뒤 데이터에 정의된 보정을 얹는다.
	NewActor->SetActorRelativeTransform(Entry.AttachTransform);
	
	TArray<UPrimitiveComponent*> Primitives;
	NewActor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->SetOnlyOwnerSee(bOnlyOwnerSee);
		Primitive->SetOwnerNoSee(bOwnerNoSee);
		// 1P 무기는 그림자를 끄고, 그림자는 3P 쪽이 담당한다.
		Primitive->SetCastShadow(!bOnlyOwnerSee);
	}

	return NewActor;
}

// ---------------------------------------------------------------------------
// 애님 레이어 / 몽타주
// ---------------------------------------------------------------------------

void UKernelCosmeticComponent::ApplyWeaponLayer(TSubclassOf<UAnimInstance> Layer1P, TSubclassOf<UAnimInstance> Layer3P)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	CurrentLayer1P = Layer1P;
	CurrentLayer3P = Layer3P;

	OnRep_WeaponLayers();
}

void UKernelCosmeticComponent::OnRep_WeaponLayers()
{
	IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner());

	if (CosmeticTarget)
	{
		if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
		{
			Mesh1P->UnlinkAnimClassLayers(nullptr);
			if (CurrentLayer1P)
			{
				Mesh1P->LinkAnimClassLayers(CurrentLayer1P);
			}
		}
	}

	if (ACharacter* Character = GetOwner<ACharacter>())
	{
		if (USkeletalMeshComponent* Mesh3P = Character->GetMesh())
		{
			Mesh3P->UnlinkAnimClassLayers(nullptr);
			if (CurrentLayer3P)
			{
				Mesh3P->LinkAnimClassLayers(CurrentLayer3P);
			}
		}
	}
}

void UKernelCosmeticComponent::PlayEquipMontage(UAnimMontage* Montage)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Montage)
	{
		return;
	}

	EquipMontage = Montage;
	++EquipMontageCounter; // 같은 무기를 연속 장착해도 OnRep 이 뜨도록
	OnRep_EquipMontage();
}

void UKernelCosmeticComponent::OnRep_EquipMontage()
{
	if (!EquipMontage)
	{
		return;
	}

	IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner());
	if (!CosmeticTarget)
	{
		return;
	}

	ACharacter* Character = GetOwner<ACharacter>();

	if (Character && Character->IsLocallyControlled())
	{
		if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
		{
			if (UAnimInstance* Anim1P = Mesh1P->GetAnimInstance())
			{
				Anim1P->Montage_Play(EquipMontage);
			}
		}
	}

	if (USkeletalMeshComponent* Mesh3P = Character ? Character->GetMesh() : nullptr)
	{
		if (UAnimInstance* Anim3P = Mesh3P->GetAnimInstance())
		{
			Anim3P->Montage_Play(EquipMontage);
		}
	}
}