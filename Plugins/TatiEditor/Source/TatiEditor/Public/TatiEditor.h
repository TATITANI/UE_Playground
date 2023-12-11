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
#pragma  endregion

#pragma  region  LevelEditorMenuExtension
	void InitLevelEditorExtension();

	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList,
	                                                    const TArray<AActor*> SelectedActors);

	void AddLevelMenuEntry(FMenuBuilder& MenuBuilder);
	void OnLockActorSelectionButtonClicked();
	void OnUnlockActorSelectionButtonClicked();

#pragma  endregion

#pragma  region SelectionLock
	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);

	void LockActorSelection(AActor* TargetActor);
	void UnlockActorSelection(AActor* TargetActor);
	bool CheckIsActorSelectionLocked(AActor* TargetActor);

	static const FName SelectionLockTagName;
		
#pragma  endregion

	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsytem;
	bool GetEditorActorSubsystem();
	
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
