// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class manipulator : ModuleRules
{
	public manipulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UnrealCV" });
		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealCV" });

	}
}
