// Fill out your copyright notice in the Description page of Project Settings.


#include "KernelCharacter/Hero/KernelHeroComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KernelCharacter/Input/KernelInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayAbility/KernelGameplayTags.h"
#include "GameplayAbility/KernelAbilitySystemComponent.h"
#include "KernelCharacter/KernelPawnExtensionComponent.h"
#include "KernelCharacter/KernelPlayerController.h"
#include "Messages/KernelVerbMessage.h"

UKernelHeroComponent::UKernelHeroComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKernelHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKernelHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled()) return;
	
	// Set Mapping context
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (PC)
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
                
			if (Subsystem && DefaultMappingContext)
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogInput, Warning, TEXT("Set Mapping Context for : %s"), *LP->GetName());
			}
		}
	}
	
	PawnExtComp = Pawn->FindComponentByClass<UKernelPawnExtensionComponent>();
	if (!PawnExtComp)
	{
		UE_LOG(LogTemp,Warning,TEXT("[HeroComponent] PawnExtComp is invalid"))
		return;
	}
	
	KernelASC = PawnExtComp->GetKernelAbilitySystemComponent();
	if (!KernelASC)
	{
		UE_LOG(LogTemp,Warning,TEXT("[HeroComponent] KernelASC is invalid"))
		return;
	}
	
	// Clear Abilities before binding for avoid duplicate bind.
	KernelASC->ClearInputAbility();
	
	// Bind Abilities
	if (UKernelInputComponent* KernelIC = Cast<UKernelInputComponent>(PlayerInputComponent))
	{
		TArray<uint32> BindHandles;
		KernelIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased,BindHandles);
		
		KernelIC->BindNativeAction(InputConfig,TAG_Input_WeaponSwap, ETriggerEvent::Started, this, &ThisClass::Input_SwapSlot,true);
		KernelIC->BindNativeAction(InputConfig,TAG_Input_MouseLook, ETriggerEvent::Triggered, this, &ThisClass::Input_MouseLook,true);
		KernelIC->BindNativeAction(InputConfig, TAG_Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, true);
		KernelIC->BindNativeAction(InputConfig,TAG_Input_Inventory, ETriggerEvent::Started, this, &ThisClass::Input_Inventory,true);
		KernelIC->BindNativeAction(InputConfig,TAG_Input_ScoreBoard, ETriggerEvent::Ongoing, this, &ThisClass::Input_ScoreBoard,true);
	}
	else
	{
		UE_LOG(LogInput, Error, TEXT("Kernel input component ERROR"));
	}
}

void UKernelHeroComponent::Input_SwapSlot(const FInputActionValue& InputActionValue)
{
	float AxisValue = InputActionValue.Get<float>();
	int32 TargetSlotIndex = FMath::RoundToInt(AxisValue) - 1;

	if (APawn* AvatarPawn = Cast<APawn>(GetOwner()))
	{
		FGameplayEventData Payload;
		Payload.EventMagnitude = TargetSlotIndex; 
		Payload.Instigator = AvatarPawn;

		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("Event.Weapon.Swap");
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AvatarPawn, EventTag, Payload);
        
		UE_LOG(LogTemp, Warning, TEXT("[HeroComponent] 입력 태그 수신 완료! 타겟 슬롯: %d"), TargetSlotIndex);
	}
}

void UKernelHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AKernelPlayerController* LyraController = Cast<AKernelPlayerController>(Controller))
	{
		//LyraController->SetIsAutoRunning(false);
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Input_Blocked)) return;
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		
		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UKernelHeroComponent::Input_MouseLook(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Input_Blocked)) return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UKernelHeroComponent::Input_Inventory()
{
	FKernelVerbMessage Message;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_Input_Inventory, Message);
}

void UKernelHeroComponent::Input_ScoreBoard()
{
	FKernelVerbMessage Message;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_Input_ScoreBoard, Message);
}
 
void UKernelHeroComponent::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;
	
	if (!PawnExtComp) return;
	if (!KernelASC) return;

	KernelASC->AbilityInputTagPressed(InputTag);
}

void UKernelHeroComponent::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;
	
	if (!PawnExtComp) return;
	if (!KernelASC) return;
	
	KernelASC->AbilityInputTagReleased(InputTag);
}

