// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlaygroundServerTarget : TargetRules
{
	public PlaygroundServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.AddRange(new[] { "Playground" });
	}
}