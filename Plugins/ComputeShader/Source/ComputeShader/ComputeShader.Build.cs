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
			new string[] {
				// ... add public include paths required here ...
			}
		);  
		
		
		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("TargetPlatform");
		}
		PublicDependencyModuleNames.Add("Core");
		PublicDependencyModuleNames.Add("Engine");
		PublicDependencyModuleNames.Add("MaterialShaderQualitySettings");
		PublicDependencyModuleNames.Add("Renderer");
		PublicDependencyModuleNames.Add("RenderCore");
		PublicDependencyModuleNames.Add("RHI");

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Renderer",
			"RenderCore",
			"RHI",
			"Projects",
		});
		
		if (Target.bBuildEditor == true)
		{

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"SlateCore",
					"Slate"
				}
			);
		
		}
	} 

}