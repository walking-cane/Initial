// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/KernelMapDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "KernelMapDefinition"

EDataValidationResult UKernelMapDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (DisplayName.IsEmpty())
	{
		Context.AddWarning(LOCTEXT("NoDisplayName", "DisplayName이 비어 있습니다 — UI에 빈 칸으로 표시됩니다."));
	}

	if (LevelPool.IsEmpty())
	{
		Context.AddError(LOCTEXT("EmptyLevelPool", "LevelPool이 비어 있습니다 — 이 테마는 선택할 수 없습니다."));
		Result = EDataValidationResult::Invalid;
	}

	for (int32 i = 0; i < LevelPool.Num(); ++i)
	{
		if (LevelPool[i].IsNull())
		{
			Context.AddError(FText::Format(
				LOCTEXT("NullLevelSlot", "LevelPool[{0}]에 레벨이 지정되지 않았습니다."),
				FText::AsNumber(i)));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE
#endif