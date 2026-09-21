// KernelLevelPreloadVolume.cpp
#include "Tutorial/KernelLevelPreloadVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "KernelCharacter/Hero/KernelHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

AKernelLevelPreloadVolume::AKernelLevelPreloadVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	PlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
	SetRootComponent(PlayerCollision);
	PlayerCollision->SetCollisionProfileName(TEXT("Trigger"));
}

void AKernelLevelPreloadVolume::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlap);
}

void AKernelLevelPreloadVolume::StartCameraFade(bool bReverse)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(
			bReverse ? 1.f : 0.f,
			bReverse ? 0.f : 1.f,
			FadeDuration, 
			FLinearColor::White,
			true,
			/*bHoldWhenFinished=*/ true);
	}
}

void AKernelLevelPreloadVolume::PreloadLevel()
{
	bool bSuccess = false;

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	
	ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this, TargetLevel, FVector::ZeroVector, FRotator::ZeroRotator, bSuccess);

	UE_LOG(LogTemp, Warning, TEXT("[PreloadVolume] 로드 중: %s"),
		*TargetLevel.GetAssetName());
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreloadVolume] 로드 완료: %s"),
		*TargetLevel.GetAssetName());
		
		UGameplayStatics::UnloadStreamLevel(
			this, 
			FName("IntroLevel"),
			FLatentActionInfo(), 
			true);
		
		UE_LOG(LogTemp, Warning, TEXT("[PreloadVolume] IntroLevel 제거 완료"));
		
		StartCameraFade(true);
		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("[PreloadVolume] 로드 실패: %s"),
		*TargetLevel.GetAssetName());
}

void AKernelLevelPreloadVolume::OnOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActivated || !Cast<AKernelHeroCharacter>(OtherActor)) return;

	if (TargetLevel.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[PreloadVolume] TargetLevel 미지정 — %s"), *GetName());
		return;
	}
	
	bActivated = true;
	StartCameraFade(false);
	
	GetWorldTimerManager().SetTimer(StreamingTimerHandle, this, &ThisClass::PreloadLevel,
		FadeDuration + 0.5f, false);
}
