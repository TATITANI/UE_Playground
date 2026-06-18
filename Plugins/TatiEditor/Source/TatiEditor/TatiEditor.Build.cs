// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TatiEditor : ModuleRules
{
	public TatiEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new[]
			{
				Path.GetFullPath(Target.RelativeEnginePath) + "Source/Editor/Blutility/Private",
				Path.GetFullPath(Target.RelativeEnginePath) + "Source/Editor/SceneOutliner/Private"
				// ... add other private include paths required here ...
			}
		);

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", "Blutility", "EditorScriptingUtilities", "UMG",
				"Niagara", "UnrealEd", "AssetTools", "ContentBrowser", "InputCore",
				"Projects", "SceneOutliner"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"SceneOutliner"
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}