// KernelItemFragment_Recoil.h
#pragma once

#include "KernelItemFragment.h"
#include "KernelItemFragment_Recoil.generated.h"

class UCurveVector;

UCLASS()
class KERNEL_API UKernelItemFragment_Recoil : public UKernelItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	TObjectPtr<UCurveVector> RecoilCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float KickAmount = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float MaxKickAmount = 5.f;
};