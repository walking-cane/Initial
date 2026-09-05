// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/KernelItemPickup.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Affixes/KernelAffixDefinition.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "Item/KernelItemFragment_Cosmetic.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemManager.h"
#include "KernelCharacter/KernelPlayerController.h"
#include "Messages/KernelInteractionMessage.h"
#include "Net/UnrealNetwork.h"

AKernelItemPickup::AKernelItemPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnMeshComp1 = CreateDefaultSubobject<USkeletalMeshComponent>("SpawnMeshComp");
	SetRootComponent(SpawnMeshComp1); 
	
	RarityFX1 = CreateDefaultSubobject<UNiagaraComponent>("RarityFX");
	RarityFX1->bAutoActivate = false;
	
	SetReplicateMovement(false);
}

void AKernelItemPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKernelItemPickup, Roll);
}

void AKernelItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
	OriginVector = SpawnMeshComp1->GetComponentLocation();
	
	if (RarityFX1 && RarityFX1->GetAsset())
	{
		RarityFX1->Activate(true);
		RarityFX1->SetRelativeLocation(OriginVector);
	}
}

void AKernelItemPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	MovedDistance = FVector::Dist(OriginVector, SpawnMeshComp1->GetComponentLocation());
	
	if (MovedDistance > 5.f) MoveAmount *= -1.f;
	
	SpawnMeshComp1->AddLocalOffset(FVector(0.f, 0.f, MoveAmount));
	SpawnMeshComp1->AddLocalRotation(FRotator(0.f, 1.f, 0.f));
}

void AKernelItemPickup::ApplyRoll(const FKernelItemData& InRoll)
{
	if (!InRoll.IsValid()) return;

	Roll = InRoll;

	ItemInstance = NewObject<UKernelItemInstance>(this);
	ItemInstance->InitFromRollResult(Roll);

	if (const UKernelItemFragment_Cosmetic* Cos = ItemInstance->FindFragmentByClass<UKernelItemFragment_Cosmetic>())
	{
		SpawnMeshComp1->SetSkeletalMesh(Cos->SpawnMesh.LoadSynchronous());
	}
	OriginVector = SpawnMeshComp1->GetComponentLocation();
	
	if (RarityFX1)
	{
		if (const TSoftObjectPtr<UNiagaraSystem>* Found = RarityFXAssets.Find(Roll.Rarity))
		{
			if (UNiagaraSystem* FX = Found->LoadSynchronous())
			{
				RarityFX1->SetAsset(FX);   // 비활성 상태에서 교체
			}
		}
	}
}

void AKernelItemPickup::OnInteract(APawn* Pawn)
{
	UE_LOG(LogTemp,Warning,TEXT("ItemPickup::OnInteract"))
	if (!Pawn) return;
	
	UKernelItemManager* Mgr = Pawn->FindComponentByClass<UKernelItemManager>();
	if (!Mgr) return;

	FKernelInteractionMessage Message;
	UGameplayMessageSubsystem& System = UGameplayMessageSubsystem::Get(this);
	
	if (bLocalOnly)
	{
		// 개인 드롭: 서버는 이 액터를 모른다. ID만 보내고 즉시 로컬 제거.
		// GA_Interact가 LocalPredicted라 이 분기는 소유 클라에서 실행된다.
		if (!Pawn->IsLocallyControlled()) return;

		// 클라 사전 체크 — 서버가 어차피 거부할 걸 미리 막아 픽업이 사라지는 걸 방지
		if (!Mgr->CanAcceptItem(ItemInstance))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Pickup] 무기 슬롯이 가득 참"));
			return;
		}
		
		if (AKernelPlayerController* PC = Cast<AKernelPlayerController>(Pawn->GetController()))
		{
			PC->Server_TakeDrop(DropId);
		}
		
		System.BroadcastMessage(TAG_Interact_EndFocus, Message);
		Destroy();
		return;
	}
	
	if (!HasAuthority() || !ItemInstance) return; // 공유 픽업: 기존 서버 권위 경로

	if (!Mgr->AddItemToInventory(ItemInstance))
	{
		return;   // 거부 시 픽업을 월드에 남긴다
	}

	ItemInstance->Rename(nullptr, Mgr->GetOwner());
	System.BroadcastMessage(TAG_Interact_EndFocus, Message);
	Destroy();
}

void AKernelItemPickup::OnFocus(APawn* Pawn)
{
	if (!Pawn || !Pawn->IsLocallyControlled() || !SpawnMeshComp1) return;

	SpawnMeshComp1->SetCustomDepthStencilValue(GetStencilForRarity(Roll.Rarity));
	SpawnMeshComp1->SetRenderCustomDepth(true);
	
	FKernelInteractionMessage Message;
	Message.Description = FText::FromString("to Pickup");
	
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_OnFocus, Message);
}

void AKernelItemPickup::OnEndFocus(APawn* Pawn)
{
	if (!Pawn || !Pawn->IsLocallyControlled() || !SpawnMeshComp1) return;
	SpawnMeshComp1->SetRenderCustomDepth(false);
	
	FKernelInteractionMessage Message;
	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage(TAG_Interact_EndFocus, Message);
}

FText AKernelItemPickup::GetInteractName()
{
	return Roll.ItemDef ? Roll.ItemDef->DisplayName : FText::GetEmpty();
}

FText AKernelItemPickup::GetInteractDescription()
{
	return Roll.ItemDef ? Roll.ItemDef->Description : FText::GetEmpty();
}

UTexture2D* AKernelItemPickup::GetInteractIcon()
{
	return Roll.ItemDef ? Roll.ItemDef->Icon.LoadSynchronous() : nullptr;
}

FVector AKernelItemPickup::GetInteractLocation()
{
	return Roll.ItemDef ? GetActorLocation() : FVector::ZeroVector;
}

TArray<FKernelAffixDetail> AKernelItemPickup::GetInteractDetails()
{
	UE_LOG(LogTemp, Warning, TEXT("[Details] %s / %.1f%%"),
	*GetName(), Roll.Affixes.Num() > 0 ? Roll.Affixes[0].RolledChance * 100.f : -1.f);
	
	TArray<FKernelAffixDetail> Result;
	for (const FKernelAppliedAffix& Applied : Roll.Affixes)
	{
		if (!Applied.Affix) continue;

		FKernelAffixDetail Entry;
		Entry.AffixName = Applied.Affix->DisplayName;
		Entry.AffixDescription = Applied.Affix->BuildDescription(Applied.RolledChance); // 개체별 확률 치환
		Entry.AffixIcon = Applied.Affix->Icon.LoadSynchronous();
		Entry.AffixDef = Applied.Affix;
		Entry.RerollsUsed = Applied.RerollsUsed;
		Result.Add(Entry);
	}
	return Result;
}
