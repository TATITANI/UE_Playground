// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlaygroundTarget : TargetRules
{
	public PlaygroundTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.AddRange(new[] { "Playground", "ComputeShader" });
	}
}