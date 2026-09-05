#pragma once

#include "CoreMinimal.h"
#include "KernelCrossHairStyle.generated.h"

USTRUCT(BlueprintType)
struct KERNEL_API FKernelCrosshairStyle
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly) float Gap = 8.f;            // 중심 ~ 날개 안쪽
	UPROPERTY(EditDefaultsOnly) float WingLength = 12.f;    // 0이면 날개 없음
	UPROPERTY(EditDefaultsOnly) float WingThickness = 2.f;
	UPROPERTY(EditDefaultsOnly) float DotSize = 2.f;        // 0이면 점 없음
	UPROPERTY(EditDefaultsOnly) float RingRadius = 0.f;     // 레이저용 원, 0이면 없음
	UPROPERTY(EditDefaultsOnly) float RingThickness = 0.f;
	UPROPERTY(EditDefaultsOnly) float Rotation = 0.f;       // 45도 회전 등
	UPROPERTY(EditDefaultsOnly) FLinearColor Color = FLinearColor::White;

	/** 두 스타일 사이 보간 */
	static FKernelCrosshairStyle Lerp(const FKernelCrosshairStyle& A, const FKernelCrosshairStyle& B, float Alpha)
	{
		FKernelCrosshairStyle R;
		R.Gap            = FMath::Lerp(A.Gap, B.Gap, Alpha);
		R.WingLength     = FMath::Lerp(A.WingLength, B.WingLength, Alpha);
		R.WingThickness  = FMath::Lerp(A.WingThickness, B.WingThickness, Alpha);
		R.DotSize        = FMath::Lerp(A.DotSize, B.DotSize, Alpha);
		R.RingRadius     = FMath::Lerp(A.RingRadius, B.RingRadius, Alpha);
		R.RingThickness  = FMath::Lerp(A.RingThickness, B.RingThickness, Alpha);
		R.Rotation       = FMath::Lerp(A.Rotation, B.Rotation, Alpha);
		R.Color          = FMath::Lerp(A.Color, B.Color, Alpha);
		return R;
	}
};