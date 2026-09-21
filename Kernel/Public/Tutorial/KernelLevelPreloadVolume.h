// KernelLevelPreloadVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KernelLevelPreloadVolume.generated.h"

class ULevelStreamingDynamic;
class UBoxComponent;

UCLASS()
class KERNEL_API AKernelLevelPreloadVolume : public AActor
{
	GENERATED_BODY()

public:
	AKernelLevelPreloadVolume();

	const TSoftObjectPtr<UWorld>& GetTargetLevel() const { return TargetLevel; }

protected:
	virtual void BeginPlay() override;
	
	void StartCameraFade(bool bReverse);
	void PreloadLevel();

	UPROPERTY(VisibleAnywhere, Category = "Level")
	TObjectPtr<UBoxComponent> PlayerCollision;

	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> TargetLevel;

private:
	UPROPERTY(EditDefaultsOnly)
	float FadeDuration;
	
	bool bActivated = false;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	FTimerHandle StreamingTimerHandle;
};