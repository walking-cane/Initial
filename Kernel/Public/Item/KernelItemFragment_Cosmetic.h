#pragma once

#include "KernelItemFragment.h"
#include "KernelItemFragment_Cosmetic.generated.h"

class AKernelItemPickup;

/**
 * 무기 메시 액터 하나를 어디에 어떻게 붙일지 정의하는 규칙.
 * 건틀릿/너클처럼 좌우 2개가 필요한 무기는 이 규칙을 2개 등록한다.
 */
USTRUCT(BlueprintType)
struct KERNEL_API FKernelWeaponAttachRule
{
	GENERATED_BODY()

	/** 스폰할 무기 액터 클래스. 메시 컴포넌트가 아니라 액터다. */
	UPROPERTY(EditDefaultsOnly, Category = "AttachRule")
	TSoftClassPtr<AActor> WeaponActorClass;

	/** 1인칭 팔 메시에서 붙을 소켓(또는 본) 이름. */
	UPROPERTY(EditDefaultsOnly, Category = "AttachRule")
	FName AttachSocket1P = NAME_None;

	/** 3인칭 바디 메시에서 붙을 소켓(또는 본) 이름. */
	UPROPERTY(EditDefaultsOnly, Category = "AttachRule")
	FName AttachSocket3P = NAME_None;

	/** 소켓 기준 추가 보정. AI 생성 메시의 피벗/축 어긋남을 여기서 잡는다. */
	UPROPERTY(EditDefaultsOnly, Category = "AttachRule")
	FTransform AttachTransform = FTransform::Identity;
};

UCLASS()
class KERNEL_API UKernelItemFragment_Cosmetic : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TObjectPtr<USkeletalMesh> SpawnActorMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TSubclassOf<AKernelItemPickup> PickupActorClass;

	/** 장착 시 스폰/부착할 무기 액터 목록. 보통 1개, 쌍수 무기는 2개. */
	UPROPERTY(EditDefaultsOnly, Category = "Attach")
	TArray<FKernelWeaponAttachRule> AttachRules;
};