// Copyright Epic Games, Inc. All Rights Reserved.

#include "TatiEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include  "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FTatiEditorModule"

void FTatiEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogTemp, Warning, TEXT("StartupModule"));
	InitCBMenuExtension();
}

void FTatiEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FTatiEditorModule::InitCBMenuExtension()
{
	UE_LOG(LogTemp, Warning, TEXT("InitCBMenuExtension"));

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	CBModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FTatiEditorModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FTatiEditorModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomCBMenuExtender"));

	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		FolderPathsSelected = SelectedPaths;
		
		MenuExtender->AddMenuExtension(FName("Delete"),
		                               EExtensionHook::After,
		                               TSharedPtr<FUICommandList>(),
		                               FMenuExtensionDelegate::CreateRaw(this, &FTatiEditorModule::AddCBMenuEntry));
	}


	return MenuExtender;
}


void FTatiEditorModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all unused assets under folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete all empty folders")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnDeleteEmtpyFoldersButtonClicked)

	);
}

void FTatiEditorModule::OnDeleteUnusedAssetButtonClicked()
{
	DebugHeader::Print(TEXT("Working"), FColor::Green);
	if (FolderPathsSelected.Num() != 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("you can only do this to one folder"));
		return;
	}
	DebugHeader::Print(FString::Printf(TEXT("Currently selected folder : %s"), *FolderPathsSelected[0]), FColor::Green);

	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	if (AssetsPathNames.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Asset found under selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmResult =
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, FString::Printf(TEXT("총 %d 개의 에셋을 체크함. \n 진행할것?"), AssetsPathNames.Num()));

	if (ConfirmResult == EAppReturnType::No)
		return;

	TArray<FAssetData> UnusedAssetsData;
	for (const FString& AssetPathName : AssetsPathNames)
	{
		//  don't touch root folder
		if (AssetPathName.Contains("Developers") ||
			AssetPathName.Contains("Collections") ||
			AssetPathName.Contains("__ExternalActors__") ||
			AssetPathName.Contains("__ExternalObjects__"))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsData.Num() > 0)
	{
		const int32 DeleteCnt = ObjectTools::DeleteAssets(UnusedAssetsData);
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, FString::Printf(TEXT("Delete %d Assets"), DeleteCnt), false);
	}
}

void FTatiEditorModule::OnDeleteEmtpyFoldersButtonClicked()
{
	FixUpRedirectors();
	if (FolderPathsSelected.Num() != 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("you can only do this to one folder"));
		return;
	}
	
	TArray<FString> FolderPaths = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
	uint32 Count = 0;

	TArray<FString> EmptyFolderPaths;
	FString FolderNames = "";
	for (const FString& FolderPath : FolderPaths)
	{
		//  don't touch root folder
		if (FolderPath.Contains("Developers") ||
			FolderPath.Contains("Collections") ||
			FolderPath.Contains("__ExternalActors__") ||
			FolderPath.Contains("__ExternalObjects__"))
		{
			continue;
		}

		if (UEditorAssetLibrary::DoesDirectoryExist(FolderPath) == false)
		{
			continue;
		}

		if (UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath) == false)
		{
			EmptyFolderPaths.Add(FolderPath);
			FolderNames += FolderPath + "\n";
			// EmptyFolderPaths.Add(TEXT("\n"));
		}
	}

	if (EmptyFolderPaths.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("빈 폴더 없음"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(EAppMsgType::OkCancel,
	                                                                FString::Printf(TEXT("빈 폴더 %s"), *FolderNames));

	if (ConfirmResult == EAppReturnType::Cancel)
	{
		return;
	}

	uint32 DeleteCnt = 0;
	for (auto EmptyFolderPath : EmptyFolderPaths)
	{
		const bool bDelete = UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath);
		bDelete ? DeleteCnt++ : DebugHeader::Print(TEXT("폴더 삭제 실패 : ") + EmptyFolderPath, FColor::Red);
	}

	DebugHeader::ShowMsgDialog(EAppMsgType::Ok, FString::Printf(TEXT("폴더 %d 개 삭제"), DeleteCnt), false);
}

void FTatiEditorModule::FixUpRedirectors()
{
	// NOTE
	//애셋 레지스트리 (Asset Registry)는 에디터가 로드되면서 로드되지 않은 애셋에 대한 정보를 비동기적으로 그러모으는 에디터 서브시스템입니다
	// https://docs.unrealengine.com/4.27/ko/ProgrammingAndScripting/ProgrammingWithCPP/Assets/Registry/

	TArray<UObjectRedirector*> RedirectorsToFixArray;
	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// GetAssets() 를 부를 때 FARFilter 를 제공하여 여러 범주별로 필터링되는 애셋 목록을 만들 수 있습니다. 필터는 여러 성분으로 구성됩니다:
	FARFilter Filter;
	Filter.bRecursivePaths = true; // true - 하위경로 탐색, false - 지정된 경로만 탐색
	Filter.PackagePaths.Emplace("/Game"); // "/Game" - 콘텐츠 폴더
	Filter.ClassPaths.Emplace("/Script/Engine.ObjectRedirector");
	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}

#pragma  endregion


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTatiEditorModule, TatiEditor)
