#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KernelHeroWeaponComponent.generated.h"

class UCurveVector;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UKernelHeroWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKernelHeroWeaponComponent();
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly) 
	float RecoilMultiplier = 2.f;
	
	void StartRecoil();
	void StopRecoil();
	void ApplyRecoil(float DeltaTime);
	
protected:
	UPROPERTY()
	TObjectPtr<UCurveVector> CurrentRecoilCurve;
	
	float TimeSinceFired = 0.f;
	FVector2D PreviousRecoilValue = FVector2D::ZeroVector;
};