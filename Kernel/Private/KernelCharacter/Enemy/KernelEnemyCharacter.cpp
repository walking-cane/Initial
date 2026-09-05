// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/Enemy/KernelEnemyCharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "UI/EnemyHealthbarWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "GameplayAbility/Attributes/KernelCombatAttributeSet.h"
#include "GameplayAbility/Attributes/KernelHealthAttributeSet.h"
#include "Item/KernelAffixRollLibrary.h"
#include "KernelCharacter/KernelHealthComponent.h"
#include "KernelCharacter/KernelPawnExtensionComponent.h"
#include "KernelCharacter/KernelPlayerController.h"

AKernelEnemyCharacter::AKernelEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	KernelASC = CreateDefaultSubobject<UKernelAbilitySystemComponent>("ASC");
	CombatSet = CreateDefaultSubobject<UKernelCombatAttributeSet>(TEXT("CombatSet"));
	HealthSet = CreateDefaultSubobject<UKernelHealthAttributeSet>(TEXT("HealthSet"));
	
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBarComponent");
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawAtDesiredSize(true);
	HealthBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	HealthBarComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
}

void AKernelEnemyCharacter::InitializeASC()
{
	KernelASC->InitAbilityActorInfo(this, this);
		
	if (KernelPawnExtComp)
	{
		KernelPawnExtComp->InitializeExtension(KernelASC);
		HealthComp->InitializeWithAbilitySystem(KernelASC);
			
		GrantDefaultAbilities();
	}
	
	if (DissolveCurve && DissolveTimeline)
	{
		FOnTimelineFloat Progress;
		Progress.BindUFunction(this, FName("HandleDissolveProgress"));
		DissolveTimeline->AddInterpFloat(DissolveCurve, Progress);
	}
}

void AKernelEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TryInitHealthBar();
	InitializeASC();
}

UAnimMontage* AKernelEnemyCharacter::GetDeathMontage()
{
	if (DeathMontage)
	{
		return DeathMontage;
	}
	return nullptr;
}

void AKernelEnemyCharacter::DropItemOnDeath()
{
	if (!HasAuthority()) return;
	
	if (DropTable.Num() == 0) return;

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return;

	for (APlayerState* PS : GS->PlayerArray)
	{
		AKernelPlayerController* PC = PS ? Cast<AKernelPlayerController>(PS->GetOwner()) : nullptr;
		if (!PC) continue;

		if (FMath::FRand() > DropChance) continue;   // 각자 독립 판정

		UKernelItemDefinition* Def = DropTable[FMath::RandRange(0, DropTable.Num() - 1)];
		const FKernelItemData Roll = UKernelAffixRollLibrary::RollItem(Def, RarityWeights);
		if (!Roll.IsValid()) continue;

		// 플레이어마다 조금씩 흩어지게
		FTransform Xform = GetActorTransform();
		Xform.AddToTranslation(FVector(FMath::VRand().GetSafeNormal2D() * FMath::FRandRange(30.f, 90.f)));
		Xform.SetScale3D(FVector::OneVector);

		PC->GrantLocalDrop(Roll, Xform);
		UE_LOG(LogTemp,Warning,TEXT("[Enemy] drop item"))
	}
}

void AKernelEnemyCharacter::HandleDeathStarted()
{
	AActor* Enemy = GetOwner();
	if (!Owner) return;
	
	if (ACharacter* C = Cast<ACharacter>(Owner))
	{ 
		C->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		C->GetMesh()->SetCollisionProfileName("NoCollision");
		C->GetCharacterMovement()->DisableMovement();
		C->GetCharacterMovement()->StopMovementImmediately();
	}

	if (Owner->HasAuthority())
	{
		if (AAIController* AIC = Cast<AAIController>(Owner->GetInstigatorController()))
		{
			if (UBrainComponent* Brain = AIC->GetBrainComponent())
			{
				Brain->StopLogic(TEXT("Death"));
			}
			AIC->StopMovement();
		}
	}
}

void AKernelEnemyCharacter::TryInitHealthBar()
{
	if (!HealthBarComponent) return;

	// 1. 위젯 강제 초기화 — 클라이언트에서 lazy init로 아직 null일 수 있으므로
	HealthBarComponent->InitWidget();
	UUserWidget* RawWidget = HealthBarComponent->GetUserWidgetObject();
	UEnemyHealthBarWidget* HealthBar = Cast<UEnemyHealthBarWidget>(RawWidget);

	// 2. HealthComponent 확보 (이미 멤버로 있으면 그걸, 없으면 검색)
	if (!HealthComp)
	{
		HealthComp = FindComponentByClass<UKernelHealthComponent>();
	}

	// 위젯이나 컴포넌트가 아직 준비 안 됐으면 다음 틱 재시도
	if (!HealthBar || !HealthComp)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AKernelEnemyCharacter::TryInitHealthBar);
		return;
	}

	// 3. 위젯을 이 몹의 HealthComponent에 바인딩
	HealthBar->BindToEnemy(HealthComp);
}

void AKernelEnemyCharacter::StartDissolve(float Duration, bool bOut)
{
	if (!DissolveTimeline)
	{
		UE_LOG(LogTemp,Error,TEXT("[EnemyChar] No DissolveTimeLine"))
		return;
	}
	
	if (!DissolveCurve)
	{
		UE_LOG(LogTemp,Error,TEXT("[EnemyChar] No DissolveCurve"))
		return;
	}
	
	EnsureDissolveMIDs();

	// 커브 길이와 무관하게 원하는 지속시간에 맞춘다
	float MinT = 0.f, MaxT = 1.f;
	DissolveCurve->GetTimeRange(MinT, MaxT);

	const float CurveLen = FMath::Max(MaxT - MinT, KINDA_SMALL_NUMBER);
	DissolveTimeline->SetPlayRate(CurveLen / FMath::Max(Duration, KINDA_SMALL_NUMBER));

	bOut ? DissolveTimeline->PlayFromStart() : DissolveTimeline->ReverseFromEnd();
}

void AKernelEnemyCharacter::ResetDissolve()
{
	if (DissolveTimeline)
	{
		DissolveTimeline->Stop();
	}
	
	for (UMaterialInstanceDynamic* MID : DissolveMIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(DissolveParamName, 0.f);
		}
	}
}

void AKernelEnemyCharacter::HandleDissolveProgress(float Value)
{
	for (UMaterialInstanceDynamic* MID : DissolveMIDs)
	{
		if (MID) MID->SetScalarParameterValue(DissolveParamName, Value);
	}
}

void AKernelEnemyCharacter::EnsureDissolveMIDs()
{
	if (DissolveMIDs.Num() > 0) return;   // 한 번만 생성

	TArray<USkeletalMeshComponent*> Meshes;
	Meshes.Add(GetMesh());
	if (USkeletalMeshComponent* M1P = GetMesh1P()) Meshes.Add(M1P);

	for (USkeletalMeshComponent* M : Meshes)
	{
		if (!M) continue;

		for (int32 i = 0; i < M->GetNumMaterials(); ++i)
		{
			if (UMaterialInstanceDynamic* MID = M->CreateAndSetMaterialInstanceDynamic(i))
			{
				DissolveMIDs.Add(MID);
			}
		}
	}
}