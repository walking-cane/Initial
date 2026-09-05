// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KernelVerbMessage.generated.h"

/**
 * Kernel 프로젝트 전용 경량화 Verb Message
 * GMS(Global Message Subsystem)를 통해 이벤트를 방송할 때 사용합니다.
 */
USTRUCT(BlueprintType)
struct KERNEL_API FKernelVerbMessage
{
    GENERATED_BODY()

    // 1. 무슨 사건인가? (필수)
    // 예: "Message.Combat.Damage", "Message.Combat.Death"
    UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
    FGameplayTag Verb;

    // 2. 가해자 (Instigator - 누가 때렸는가?)
    // 액터뿐만 아니라 컨트롤러, 무기 등도 담을 수 있게 UObject로 설정
    UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
    TObjectPtr<UObject> Instigator = nullptr;

    // 3. 피해자 (Target - 누가 맞았는가?)
    UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
    TObjectPtr<UObject> Target = nullptr;

    // 4. 데미지 수치나 회복량 (Magnitude)
    UPROPERTY(BlueprintReadWrite, Category="Kernel|Message")
    double Magnitude = 0.0;

    // 관련된 수치나 태그들은 무엇인가? (예: 데미지 50, 혹은 특수 태그들)
    UPROPERTY(BlueprintReadWrite)
    FGameplayTagContainer Tags;
};