// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTatiEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#pragma region ContentBrowserMenuExtension

	void InitCBMenuExtension();

	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetButtonClicked();

	void OnDeleteEmtpyFoldersButtonClicked();
	void OnAdvanceDeleteButtonClicked();

private:
	TArray<FString> FolderPathsSelected;

public:
	static void FixUpRedirectors();

#pragma region CustomEditorTab
	void RegisterAdvanceDeletionTab();
	TSharedRef<SDockTab> OnSpawnDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetDatasUnderSelectedFolder();

#pragma  endregion

#pragma region ProcessDataForAdvanceDeletionTab

	bool DeleteSingleAssetForAssetList(const FAssetData& DeletingAssetData);
	void ListUnusedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetDatas);
	void ListSameNamedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNamedAssetDatas);

#pragma endregion

private:
	static const FName AdvanceDeletionName;
};
