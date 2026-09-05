// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/Hero/KernelHeroCharacter.h"

#include "KernelHeroWeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "KernelCharacter/Hero/KernelHeroComponent.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Interaction/KernelInteractionComponent.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemManager.h"
#include "Item/KernelAffixRollLibrary.h"
#include "Cosmetics/KernelCosmeticComponent.h"
#include "KernelCharacter/KernelHealthComponent.h"
#include "KernelCharacter/KernelPawnExtensionComponent.h"
#include "KernelCharacter/KernelPlayerState.h"
#include "KernelCharacter/Hero/KernelCharacterDefinition.h"
#include "Movement/KernelCharacterMovementComponent.h"

AKernelHeroCharacter::AKernelHeroCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UKernelCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	HeroComp = CreateDefaultSubobject<UKernelHeroComponent>("HeroComp");
	InteractionComp = CreateDefaultSubobject<UKernelInteractionComponent>("InteractionComp");
	ItemManagerComp = CreateDefaultSubobject<UKernelItemManager>("ItemManagerComp");
	QuickBarComp = CreateDefaultSubobject<UKernelQuickBarComponent>("QuickBarComp");
	CosmeticComp = CreateDefaultSubobject<UKernelCosmeticComponent>("CosmeticComp");
	WeaponComp = CreateDefaultSubobject<UKernelHeroWeaponComponent>("WeaponComp");
	
	FirstPersonCamera1 = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera1->SetupAttachment(RootComponent);
	
	FirstPersonMesh1 = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMesh");
	FirstPersonMesh1->SetupAttachment(FirstPersonCamera1);
	
	WeaponMeshComp1P = CreateDefaultSubobject<USkeletalMeshComponent>(("WeaponMeshComp1P"));
	WeaponMeshComp1P->SetupAttachment(FirstPersonMesh1);
	WeaponMeshComp1P->SetOnlyOwnerSee(true);
	
	WeaponMeshComp3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMeshComp3P");
	WeaponMeshComp3P->SetupAttachment(GetMesh());
	WeaponMeshComp3P->SetOwnerNoSee(true);
}

void AKernelHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>())
	{
		UKernelAbilitySystemComponent* ASC = PS->GetKernelAbilitySystemComponent();
		if (!ASC) return;
		
		if (UKernelCharacterMovementComponent* KernelMoveComp = Cast<UKernelCharacterMovementComponent>(GetCharacterMovement()))
		{
			KernelMoveComp->InitializeASC(ASC);
			HealthComp->InitializeWithAbilitySystem(ASC);
		}
		
		PS->SetCharacterDefinition(DefaultCharacterDefinition);
		
		if (PS->GetCharacterDefinition())
		{
			GrantStartingItem();
		}
	}
	
	OnRep_PlayerState();
}

void AKernelHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (HeroComp)
	{
		HeroComp->InitializePlayerInput(PlayerInputComponent);
	}
}

void AKernelHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FirstPersonMesh1 && WeaponMeshComp1P)
	{
		WeaponMeshComp1P->AttachToComponent(
			FirstPersonMesh1, 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			FName("WeaponSocket"));
	}

	if (GetMesh() && WeaponMeshComp3P)
	{
		WeaponMeshComp3P->AttachToComponent(
			GetMesh(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			FName("WeaponSocket"));
		WeaponMeshComp3P->bOwnerNoSee = true;
	}
}

UAbilitySystemComponent* AKernelHeroCharacter::GetAbilitySystemComponent() const
{
	if (AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>())
	{
		return PS->GetKernelAbilitySystemComponent();
	}
	return nullptr;
}

void AKernelHeroCharacter::SetWeaponMesh(USkeletalMesh* NewWeaponMesh)
{
	WeaponMeshComp1P->SetSkeletalMesh(NewWeaponMesh);
	WeaponMeshComp3P->SetSkeletalMesh(NewWeaponMesh);
}

void AKernelHeroCharacter::OnDeath1P(UAnimMontage* DeathMontage1P)
{
	if (UAnimInstance* Anim1P = GetMesh1P()->GetAnimInstance())
	{
		Anim1P->Montage_Play(DeathMontage1P);
	}
}

UAnimMontage* AKernelHeroCharacter::GetDeathMontage()
{
	if (DeathMontage)
	{
		return DeathMontage;
	}
	
	return nullptr;
}

void AKernelHeroCharacter::GrantStartingItem()
{
	UE_LOG(LogTemp, Warning, TEXT("[Grant] Pawn=%s Auth=%d Local=%d PS=%s"),
	*GetName(), HasAuthority(), IsLocallyControlled(), *GetNameSafe(GetPlayerState()));
	
	if (!HasAuthority()) return;

	AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>();
	UKernelCharacterDefinition* Def = PS ? PS->GetCharacterDefinition() : nullptr;
	if (!Def || !Def->StartingItem || !ItemManagerComp) return;

	TMap<EItemRarity, int32> FixedRarity;
	FixedRarity.Add(Def->StartingItemRarity, 1.f);

	const FKernelItemData Roll = UKernelAffixRollLibrary::RollItem(Def->StartingItem, FixedRarity);

	UKernelItemInstance* Inst = NewObject<UKernelItemInstance>(this);
	Inst->InitFromRollResult(Roll);
	ItemManagerComp->AddItemToInventory(Inst); // 서브오브젝트 등록 + 퀵바 배치 + 자동 장착까지 기존 경로
}

void AKernelHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (AKernelPlayerState* PS = GetPlayerState<AKernelPlayerState>())
	{
		UKernelAbilitySystemComponent* ASC = PS->GetKernelAbilitySystemComponent();
		if (!ASC) return;
		
		ASC->InitAbilityActorInfo(PS, this);
	
		if (KernelPawnExtComp)
		{
			KernelPawnExtComp->InitializeExtension(ASC);
			HealthComp->InitializeWithAbilitySystem(ASC);
			
			GrantDefaultAbilities();
		}
		
		if (UKernelCharacterMovementComponent* KernelMoveComp = Cast<UKernelCharacterMovementComponent>(GetCharacterMovement()))
		{
			KernelMoveComp->InitializeASC(ASC);
		}
	}
}
