// KernelArtifactTypes.h
#pragma once

#include "CoreMinimal.h"
#include "KernelArtifactTypes.generated.h"

class UKernelArtifactDefinition;

USTRUCT(BlueprintType)
struct FKernelArtifactChoice
{
	GENERATED_BODY()

	/** 이 제안 안에서의 식별자. 클라가 무엇을 골랐는지 서버에 알릴 때 쓴다 */
	UPROPERTY(BlueprintReadOnly)
	int32 ChoiceId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UKernelArtifactDefinition> Artifact = nullptr;
};

USTRUCT(BlueprintType)
struct FKernelArtifactOffer
{
	GENERATED_BODY()

	/** 서버 원장 대조용. 위조·중복 사용 차단의 근거 */
	UPROPERTY(BlueprintReadOnly)
	int32 OfferId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	TArray<FKernelArtifactChoice> Choices;
};

/** UI로 보낼 메시지 */
USTRUCT(BlueprintType)
struct FKernelArtifactOfferMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FKernelArtifactOffer Offer;
};