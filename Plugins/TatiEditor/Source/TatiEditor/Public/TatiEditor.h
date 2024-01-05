// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TatiEditorOutlinerSave.h"
#include "Modules/ModuleManager.h"
#include "Slate/CustomTabSpawner.h"

class FTatiEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void PostLoadCallback() override;
private:
	void InitCustomTabs();
	

public:
	static TSharedPtr<SDockTab> ConstructedDockTab; // 열려있는 탭

	FCustomTabSpawner CustomTabSpawner;
	TSharedPtr<class FCBMenuExtension> CBMenuExtension;
	TSharedPtr<class FLevelEditorMenuExtension> LevelEditorMenuExtension;
	TSharedPtr<class FOutlinerExtension> OutlinerExtension;

	static const FName SelectionLockTagName;
	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsytem;

public:
	static void FixUpRedirectors();
	bool GetEditorActorSubsystem();
	void RefreshSceneOutliner();


#pragma region Asset
	bool DeleteSingleAssetForAssetList(const FAssetData& DeletingAssetData);
	void ListUnusedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetDatas);
	void ListSameNamedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNamedAssetDatas);

#pragma endregion


#pragma  region SelectionLock
	void LockActorSelection(AActor* TargetActor);
	void UnlockActorSelection(AActor* TargetActor);
	bool CheckIsActorSelectionLocked(AActor* TargetActor);

#pragma  endregion
};
