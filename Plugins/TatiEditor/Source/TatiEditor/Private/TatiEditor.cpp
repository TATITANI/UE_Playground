// Copyright Epic Games, Inc. All Rights Reserved.

#include "TatiEditor.h"

#include "AssetToolsModule.h"
#include  "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "ObjectTools.h"
#include "SceneOutlinerModule.h"
#include "TatiEditorOutlinerSave.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "CustomStyle/TatiEditorStyle.h"
#include "CustomUICommand/TatiUICommands.h"
#include "Kismet/GameplayStatics.h"
#include "Menu/CBMenuExtension.h"
#include "Menu/LevelEditorMenuExtension.h"
#include "Outliner/OutlinerExtension.h"
#include "Slate/AdvanceDeletionWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "Slate/CustomTabSpawner.h"

#define LOCTEXT_NAMESPACE "FTatiEditorModule"

const FName FTatiEditorModule::SelectionLockTagName("Locked");
TSharedPtr<SDockTab> FTatiEditorModule::ConstructedDockTab;

void FTatiEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTatiEditorStyle::InitalizeIcons();
	FTatiUICommands::Register();

	InitCustomTabs();

	CBMenuExtension = MakeShared<FCBMenuExtension>();
	LevelEditorMenuExtension = MakeShared<FLevelEditorMenuExtension>();
	OutlinerExtension = MakeShared<FOutlinerExtension>();

}

void FTatiEditorModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SAdvanceDeletionTab::AdvanceDeletionName);
	FTatiEditorStyle::Shutdown();
	FTatiUICommands::Unregister();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FTatiEditorModule::PostLoadCallback()
{
	IModuleInterface::PostLoadCallback();
	DebugHeader::PrintLog(TEXT("FTatiEditorModule::PostLoadCallback"));
}


void FTatiEditorModule::InitCustomTabs()
{
	// Advance Deleteion
	const FCustomTabSpawner::FOnConstructSlate ConstructAdvanceDeletionTab = FCustomTabSpawner::FOnConstructSlate::CreateLambda(
		[this]()-> TSharedRef<SAdvanceDeletionTab>
		{
			TArray<TSharedPtr<FAssetData>> AssetDatas;
			if (CBMenuExtension->FolderPathsSelected.Num() > 0)
				AssetDatas = CBMenuExtension->GetAllAssetDatasUnderSelectedFolder();

			return SNew(SAdvanceDeletionTab)
				.AssetDatas(AssetDatas); // 에셋 데이터를 SAdvanceDeletionTab 파라미터로 넘김
		});

	const FCustomTabSpawner::FOnCloseTab OnCloseAdvanceDeletionTab = FCustomTabSpawner::FOnCloseTab::CreateLambda([this]
	{
		CBMenuExtension->FolderPathsSelected.Empty();
	});

	CustomTabSpawner.RegisterCustomTab<SAdvanceDeletionTab>(
		SAdvanceDeletionTab::AdvanceDeletionName, ConstructAdvanceDeletionTab, OnCloseAdvanceDeletionTab);
}

// 레벨에디터에서 잠금설정을 하면 아웃라이너 잠금 체크박스에도 반영
void FTatiEditorModule::RefreshSceneOutliner()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	TSharedPtr<ISceneOutliner> SceneOutliner = LevelEditorModule.GetFirstLevelEditor()->GetSceneOutliner();
	if (SceneOutliner.IsValid())
	{
		// 아웃라이너의 ConstructRowWidget가 다시 호출되면서 체크박스 갱신
		SceneOutliner->FullRefresh();
	}
}

void FTatiEditorModule::LockActorSelection(AActor* TargetActor)
{
	if (TargetActor == nullptr)
		return;

	if (TargetActor->ActorHasTag(SelectionLockTagName) == false)
	{
		TargetActor->Tags.Add(SelectionLockTagName);
	}
}

void FTatiEditorModule::UnlockActorSelection(AActor* TargetActor)
{
	if (TargetActor == nullptr)
		return;

	if (TargetActor->ActorHasTag(SelectionLockTagName))
	{
		TargetActor->Tags.Remove(SelectionLockTagName);
	}
}


bool FTatiEditorModule::CheckIsActorSelectionLocked(AActor* TargetActor)
{
	if (TargetActor == nullptr)
		return false;

	return TargetActor->ActorHasTag(SelectionLockTagName);
}


bool FTatiEditorModule::DeleteSingleAssetForAssetList(const FAssetData& DeletingAssetData)
{
	if (ObjectTools::DeleteAssets({DeletingAssetData}) > 0)
	{
		return true;
	}

	return false;
}

void FTatiEditorModule::ListUnusedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter,
                                         TArray<TSharedPtr<FAssetData>>& OutUnusedAssetDatas)
{
	OutUnusedAssetDatas.Empty();

	for (auto AssetData : AssetDatasToFilter)
	{
		auto Reference = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData->GetObjectPathString());

		if (Reference.Num() == 0)
			OutUnusedAssetDatas.Add(AssetData);
	}
}

void FTatiEditorModule::ListSameNamedAssets(const TArray<TSharedPtr<FAssetData>> AssetDatasToFilter,
                                            TArray<TSharedPtr<FAssetData>>& OutSameNamedAssetDatas)
{
	OutSameNamedAssetDatas.Empty();

	TMap<FName, TArray<TSharedPtr<FAssetData>>> Table;
	for (auto AssetData : AssetDatasToFilter)
	{
		Table.FindOrAdd(AssetData->AssetName).Add(AssetData);
	}

	for (const auto TableItem : Table)
	{
		const auto AssetDatas = TableItem.Value;
		if (bool Overlapped = AssetDatas.Num() > 1)
		{
			for (auto AssetData : AssetDatas)
			{
				OutSameNamedAssetDatas.Add(AssetData);
			}
		}
	}
}


bool FTatiEditorModule::GetEditorActorSubsystem()
{
	if (WeakEditorActorSubsytem.IsValid() == false)
	{
		WeakEditorActorSubsytem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return WeakEditorActorSubsytem.IsValid();
}

void FTatiEditorModule::FixUpRedirectors()
{
	// NOTE
	// 애셋 레지스트리 (Asset Registry)는 에디터가 로드되면서 로드되지 않은 애셋에 대한 정보를 비동기적으로 그러모으는 에디터 서브시스템입니다
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


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTatiEditorModule, TatiEditor)
