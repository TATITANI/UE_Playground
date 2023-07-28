// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeShader.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FComputeShaderModule"

void FComputeShaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// IMPLEMENT_SHADER_TYPE에 가상주소로 사용
	FString PluginShadeDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ComputeShader"))->GetBaseDir(), TEXT("Shaders/Private"));
	AddShaderSourceDirectoryMapping(TEXT("/ComputeShaderShaders"), PluginShadeDir);
}

void FComputeShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeShaderModule, ComputeShader)