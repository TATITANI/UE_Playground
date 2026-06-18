// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ComputeShader : ModuleRules
{
	public ComputeShader(ReadOnlyTargetRules Target) : base(Target)

	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(new string[]
		{
		});

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		if (Target.bBuildEditor) PrivateDependencyModuleNames.Add("TargetPlatform");
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("Engine");
		PublicDependencyModuleNames.Add("MaterialShaderQualitySettings");
		PublicDependencyModuleNames.Add("Renderer");
		PublicDependencyModuleNames.Add("RenderCore");
		PublicDependencyModuleNames.Add("RHI");

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"CoreUObject",
			"Renderer",
			"RenderCore",
			"RHI",
			"Projects"
		});

		if (Target.bBuildEditor)
			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"SlateCore",
					"Slate"
				}
			);
	}
}