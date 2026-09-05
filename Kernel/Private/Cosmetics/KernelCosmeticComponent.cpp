// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/KernelCosmeticComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UKernelCosmeticComponent::UKernelCosmeticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UKernelCosmeticComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKernelCosmeticComponent, CurrentLayer1P);
	DOREPLIFETIME(UKernelCosmeticComponent, CurrentLayer3P);
	DOREPLIFETIME(UKernelCosmeticComponent, CurrentWeaponMesh);
	DOREPLIFETIME(UKernelCosmeticComponent, EquipMontage);
	DOREPLIFETIME(UKernelCosmeticComponent, EquipMontageCounter);
}

void UKernelCosmeticComponent::ApplyWeaponLayer(TSubclassOf<UAnimInstance> Layer1P, TSubclassOf<UAnimInstance> Layer3P)
{
	if (!GetOwner()->HasAuthority()) return;
	
	CurrentLayer1P = Layer1P;
	CurrentLayer3P = Layer3P;
	
	OnRep_WeaponLayers();
}

void UKernelCosmeticComponent::OnRep_WeaponLayers()
{
	if (CurrentLayer1P)
	{
		if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner()))
		{
			if (USkeletalMeshComponent* Mesh1P = CosmeticTarget->GetMesh1P())
			{
				Mesh1P->UnlinkAnimClassLayers(nullptr);
				Mesh1P->LinkAnimClassLayers(CurrentLayer1P);
			}
		}
	}
	
	if (CurrentLayer3P)
	{
		if (ACharacter* Character = GetOwner<ACharacter>())
		{
			if (USkeletalMeshComponent* Mesh3P = Character->GetMesh())
			{
				Mesh3P->UnlinkAnimClassLayers(nullptr);
				Mesh3P->LinkAnimClassLayers(CurrentLayer3P);
				UE_LOG(LogTemp,Warning,TEXT("3PLayerApplied"))
			}
		}
	}
}

void UKernelCosmeticComponent::PlayEquipMontage(UAnimMontage* Montage)
{
	if (!GetOwner()->HasAuthority() || !Montage) return;

	EquipMontage = Montage;
	++EquipMontageCounter; // 값을 바꿔 OnRep 강제 트리거
	OnRep_EquipMontage();  // 서버(호스트) 본인도 재생
}

void UKernelCosmeticComponent::ChangeWeapon(USkeletalMesh* NewWeaponMesh)
{
	if (!GetOwner()->HasAuthority()) return;
	UE_LOG(LogTemp, Warning, TEXT("[Cosmetic] ChangeWeapon on SERVER: %s"), *GetNameSafe(NewWeaponMesh));
	
	CurrentWeaponMesh = NewWeaponMesh;

	OnRep_WeaponMesh(); // 서버 자신의 화면에도 즉시 반영
}

void UKernelCosmeticComponent::OnRep_WeaponMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("[Cosmetic] OnRep_WeaponMesh (Authority=%d): %s"),
		GetOwner()->HasAuthority(), *GetNameSafe(CurrentWeaponMesh));
	
	if (IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner()))
	{
		if (USkeletalMeshComponent* WeaponMesh1P = CosmeticTarget->GetWeaponMesh1P())
		{
			WeaponMesh1P->SetSkeletalMesh(CurrentWeaponMesh);
		}

		if (USkeletalMeshComponent* WeaponMesh3P = CosmeticTarget->GetWeaponMesh3P())
		{
			WeaponMesh3P->SetSkeletalMesh(CurrentWeaponMesh);
		}
	}
}

void UKernelCosmeticComponent::OnRep_EquipMontage()
{
	if (!EquipMontage) return;

	IKernelCosmeticInterface* CosmeticTarget = Cast<IKernelCosmeticInterface>(GetOwner());
	if (!CosmeticTarget) return;

	ACharacter* Character = GetOwner<ACharacter>();
	
	// 1P: 이 폰을 로컬로 조종하는 클라이언트에서만 (본인 시점 팔)
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

	// 3P: 모든 곳에서 (남들이 보는 몸체). 단, 1P만 보이는 소유자 본인 화면에선 3P가 어차피 안 보이므로 재생해도 무해.
	if (USkeletalMeshComponent* Mesh3P = Character ? Character->GetMesh() : nullptr)
	{
		if (UAnimInstance* Anim3P = Mesh3P->GetAnimInstance())
		{
			Anim3P->Montage_Play(EquipMontage);
		}
	}
}