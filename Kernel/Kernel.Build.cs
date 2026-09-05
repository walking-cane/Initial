// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
// ReSharper disable UseCollectionExpression

public class Kernel : ModuleRules
{
	public Kernel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject",
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"CommonUI",
			"GameplayMessageRuntime",
			"Niagara",
			"LevelSequence",
			"MovieScene" ,
			"DeveloperSettings",
			"NavigationSystem",
		});


		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
