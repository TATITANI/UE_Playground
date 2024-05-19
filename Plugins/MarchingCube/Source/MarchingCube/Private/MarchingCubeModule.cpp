// Copyright Epic Games, Inc. All Rights Reserved.

#include "MarchingCubeModule.h"

#include "EditorModeRegistry.h"
#include "MarchingCubeEdMode.h"
#include "Tools/MarchingCubeDetailCustomization.h"
#include "MarchingCubeWorld.h"

#define LOCTEXT_NAMESPACE "MarchingCubeModule"

void FMarchingCubeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// 에디터모드
	FEditorModeRegistry::Get().RegisterMode<FMarchingCubeEdMode>(FMarchingCubeEdMode::EM_MarchingCubeViewport);

	// 커스터마이징 등록
	PropertyModule.RegisterCustomClassLayout(AMarchingCubeWorld::StaticClass()->GetFName(),
	                                         FOnGetDetailCustomizationInstance::CreateStatic(&FMarchingCubeDetailCustomization::MakeInstance));


	UE_LOG(LogTemp, Log, TEXT("FMarchingCubeModule::StartupModule"));

}


void FMarchingCubeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FEditorModeRegistry::Get().UnregisterMode(FMarchingCubeEdMode::EM_MarchingCubeViewport);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMarchingCubeModule, MarchingCubeEditorMode)
