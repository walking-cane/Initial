// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KernelMapDefinition.generated.h"

class UTexture2D;
class UWorld;

/**
 * 하나의 맵 "테마"를 정의한다. (Ice, Desert 등)
 * UI에 노출되는 이름/이미지는 테마 단위이며, 실제로 로드되는 레벨은
 * LevelPool에서 서버가 트래블 직전에 추첨한다.
 */
UCLASS()
class KERNEL_API UKernelMapDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** UI 표시용 — 테마 단위. 개별 레벨은 이름/이미지를 갖지 않음 */
	UPROPERTY(EditDefaultsOnly, Category = "Presentation")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Presentation")
	TSoftObjectPtr<UTexture2D> PreviewImage;
	UPROPERTY(EditDefaultsOnly, Category = "Presentation")
	TSoftObjectPtr<UTexture2D> SelectImage;

	UPROPERTY(EditDefaultsOnly, Category = "Presentation")
	bool bLocked = false;

	/** 이 테마가 뽑을 수 있는 레벨 후보들. 비어 있으면 테마 선택 자체가 거부된다. */
	UPROPERTY(EditDefaultsOnly, Category = "Levels")
	TArray<TSoftObjectPtr<UWorld>> LevelPool;

#if WITH_EDITOR
	/** 빈 풀 / 미지정 슬롯을 런타임이 아니라 에디터·쿡 단계에서 잡는다 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};