#pragma once

#include "CoreMinimal.h"
#include "KernelInteractionMessage.generated.h"

class UKernelAffixDefinition;

USTRUCT(BlueprintType)
struct KERNEL_API FKernelToastMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	FText ToastMessage;
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	bool IsWarning = false;
};

USTRUCT(BlueprintType)
struct KERNEL_API FKernelReadyCountMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	int32 ReadyCount = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	int32 TotalCount = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	bool bIsLocalPlayerReady = false;
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	bool bIsReady = false;
};

USTRUCT(BlueprintType)
struct KERNEL_API FKernelAffixDetail
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	FText AffixName;
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	FText AffixDescription;
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	TObjectPtr<UKernelAffixDefinition> AffixDef = nullptr;
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	int32 RerollsUsed = 0;
	UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
	TObjectPtr<UTexture2D> AffixIcon = nullptr;
};

USTRUCT(BlueprintType)
struct KERNEL_API FKernelInteractionMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FText DisplayName;
	UPROPERTY(BlueprintReadOnly) FText Description;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UTexture2D> DisplayIcon = nullptr;
	UPROPERTY(BlueprintReadOnly) FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) TSubclassOf<UUserWidget> WidgetClass = nullptr;

	/** 부가 정보 줄들 — 아이템은 접사, NPC는 다른 용도로 쓸 수 있게 일반화 */
	UPROPERTY(BlueprintReadOnly) TArray<FKernelAffixDetail> Details;
};