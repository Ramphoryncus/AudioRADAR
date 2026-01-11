// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AudioRADAR : ModuleRules
{
	public AudioRADAR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AudioRADAR",
			"AudioRADAR/Variant_Platforming",
			"AudioRADAR/Variant_Platforming/Animation",
			"AudioRADAR/Variant_Combat",
			"AudioRADAR/Variant_Combat/AI",
			"AudioRADAR/Variant_Combat/Animation",
			"AudioRADAR/Variant_Combat/Gameplay",
			"AudioRADAR/Variant_Combat/Interfaces",
			"AudioRADAR/Variant_Combat/UI",
			"AudioRADAR/Variant_SideScrolling",
			"AudioRADAR/Variant_SideScrolling/AI",
			"AudioRADAR/Variant_SideScrolling/Gameplay",
			"AudioRADAR/Variant_SideScrolling/Interfaces",
			"AudioRADAR/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
