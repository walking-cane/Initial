#include "KernelCharacter/Hero/KernelHeroWeaponComponent.h"
#include "KernelCharacter/Hero/KernelQuickBarComponent.h"
#include "Item/KernelEquipmentInstance.h"
#include "Item/KernelItemInstance.h"
#include "Item/KernelItemFragment_Recoil.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

UKernelHeroWeaponComponent::UKernelHeroWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 반동이 없을 때는 틱을 꺼두어 성능을 최적화합니다.
	PrimaryComponentTick.bStartWithTickEnabled = false; 
}

void UKernelHeroWeaponComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	ApplyRecoil(DeltaTime);
}

void UKernelHeroWeaponComponent::StartRecoil()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		UKernelQuickBarComponent* QuickBar = OwnerPawn->FindComponentByClass<UKernelQuickBarComponent>();
			
		if (QuickBar && QuickBar->GetEquippedItem() && QuickBar->GetEquippedItem()->InstigatorItem)
		{
			const UKernelItemFragment_Recoil* RecoilFrag =
				QuickBar->GetEquippedItem()->InstigatorItem->FindFragmentByClass<UKernelItemFragment_Recoil>();
					
			if (RecoilFrag && RecoilFrag->RecoilCurve)
			{
				CurrentRecoilCurve = RecoilFrag->RecoilCurve;
				TimeSinceFired = 0.0f;
				PreviousRecoilValue = FVector2D::ZeroVector;
				
				SetComponentTickEnabled(true);
			}
		}
	}
}

void UKernelHeroWeaponComponent::StopRecoil()
{
	SetComponentTickEnabled(false);
	CurrentRecoilCurve = nullptr;
}

void UKernelHeroWeaponComponent::ApplyRecoil(float DeltaTime)
{
	if (!CurrentRecoilCurve)
	{
		StopRecoil();
		return;
	}

	TimeSinceFired += DeltaTime;

	FVector RecoilVec = CurrentRecoilCurve->GetVectorValue(TimeSinceFired);
	FVector2D CurrentRecoilValue = FVector2D(RecoilVec.X, RecoilVec.Y);
	
	FVector2D RecoilDelta = CurrentRecoilValue - PreviousRecoilValue;
	PreviousRecoilValue = CurrentRecoilValue;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->Controller)
	{
		OwnerPawn->AddControllerYawInput(RecoilDelta.X * RecoilMultiplier);
		OwnerPawn->AddControllerPitchInput(RecoilDelta.Y * RecoilMultiplier);
	}
	
	float MinTime, MaxTime;
	CurrentRecoilCurve->GetTimeRange(MinTime, MaxTime);
	if (TimeSinceFired >= MaxTime)
	{
		StopRecoil();
	}
}