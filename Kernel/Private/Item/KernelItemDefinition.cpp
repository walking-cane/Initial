// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/KernelItemDefinition.h"
#include "Item/KernelItemFragment.h"

const UKernelItemFragment* UKernelItemDefinition::FindFragmentByClass(
	TSubclassOf<UKernelItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UKernelItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}