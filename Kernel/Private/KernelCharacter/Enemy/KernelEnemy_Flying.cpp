// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/Enemy/KernelEnemy_Flying.h"

AKernelEnemy_Flying::AKernelEnemy_Flying(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 활성화 해야 투사체 발사 몽타주가 거리가 멀어도 재생됨.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
}

void AKernelEnemy_Flying::BeginPlay()
{
	Super::BeginPlay();
}

void AKernelEnemy_Flying::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority()) return;   // AI는 서버에서만

	MaintainHoverHeight(DeltaSeconds);
}

void AKernelEnemy_Flying::MaintainHoverHeight(float DeltaTime)
{
	const FVector Loc = GetActorLocation();

	FHitResult Ground;
	const FVector TraceEnd = Loc - FVector(0.f, 0.f, 2000.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (!GetWorld()->LineTraceSingleByChannel(Ground, Loc, TraceEnd, ECC_WorldStatic, Params))
	{
		return;   // 바닥을 못 찾음 — 고도 유지 포기
	}

	// 천장도 확인해서 낮은 공간에서는 낮게 난다
	FHitResult Ceiling;
	const float Ceil = GetWorld()->LineTraceSingleByChannel(
		Ceiling, Loc, Loc + FVector(0.f, 0.f, 1000.f), ECC_WorldStatic, Params)
		? Ceiling.ImpactPoint.Z - 50.f
		: TNumericLimits<float>::Max();

	const float ActualHoverHeight = FMath::RandRange(
		HoverHeight - HoverHeightOffset, HoverHeight + HoverHeightOffset);
	const float DesiredZ = FMath::Min(Ground.ImpactPoint.Z + ActualHoverHeight, Ceil);
	const float NewZ = FMath::FInterpTo(Loc.Z, DesiredZ, DeltaTime, HoverInterpSpeed);

	SetActorLocation(FVector(Loc.X, Loc.Y, NewZ), true);
}