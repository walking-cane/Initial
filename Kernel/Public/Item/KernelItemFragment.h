
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KernelItemFragment.generated.h"

// EditInlineNew와 DefaultToInstanced를 통해 
// 디자이너가 데이터 에셋 내부에서 직접 생성하고 편집할 수 있게 합니다.
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class KERNEL_API UKernelItemFragment : public UObject
{
	GENERATED_BODY()
};