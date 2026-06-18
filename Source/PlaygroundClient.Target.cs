// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlaygroundClientTarget : TargetRules
{
	public PlaygroundClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.AddRange(new[] { "Playground", "ComputeShader" });
	}
}