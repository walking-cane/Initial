// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KernelReadyBox.h"

void UKernelReadyBox::SetChecked(bool bNewChecked)
{
	if (bChecked == bNewChecked) return;   // 상태가 안 바뀌면 아무것도 안 함 — 중복 재생 방지
	bChecked = bNewChecked;
	
	if (CheckAnim)
	{
		bChecked ? PlayAnimation(CheckAnim) : PlayAnimation(UnCheckAnim);
	}
}
