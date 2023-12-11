// Copyright Epic Games, Inc. All Rights Reserved.

#include "TatiEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include  "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "LevelEditor.h"
#include "ObjectTools.h"
#include "Selection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "CustomStyle/TatiEditorStyle.h"
#include "PhysicsEngine/ConstraintUtils.h"
#include "Slate/AdvanceDeletionWidget.h"
#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "FTatiEditorModule"

const FName FTatiEditorModule::AdvanceDeletionName(TEXT("AdvanceDeletion"));
const FName FTatiEditorModule::SelectionLockTagName(TEXT("Locked"));

void FTatiEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogTemp, Warning, TEXT("StartupModule"));
	InitCBMenuExtension();
	InitLevelEditorExtension();
	RegisterAdvanceDeletionTab();
	InitCustomSelectionEvent();
}

void FTatiEditorModule::ShutdownModule()
{
	FTatiEditorStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AdvanceDeletionName);
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FTatiEditorModule::InitCBMenuExtension()
{
	UE_LOG(LogTemp, Warning, TEXT("InitCBMenuExtension"));

	// 콘텐츠 브라우저 모듈 로드
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	// 콘텐츠 브라우저 컨텍스트메뉴 
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// 컨텍스트 메뉴가 열리면 호출됨
	CBModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FTatiEditorModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FTatiEditorModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomCBMenuExtender"));
	FTatiEditorStyle::InitalizeIcons();

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
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete all empty folders")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnDeleteEmtpyFoldersButtonClicked)

	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advance Delete")),
		FText::FromString(TEXT("에셋 상태별 리스트")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnAdvanceDeleteButtonClicked)

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

#pragma region CustomEditorTab

void FTatiEditorModule::OnAdvanceDeleteButtonClicked()
{
	FixUpRedirectors();
	FGlobalTabmanager::Get()->TryInvokeTab(AdvanceDeletionName);
	DebugHeader::PrintLog(TEXT("OnAdvanceDeleteButtonClicked"));
}

void FTatiEditorModule::InitLevelEditorExtension()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	TArray<FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors>& LevelEditorMenuExtenders =
		LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	LevelEditorMenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this,
		&FTatiEditorModule::CustomLevelEditorMenuExtender));
}

TSharedRef<FExtender> FTatiEditorModule::CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList,
                                                                       const TArray<AActor*> SelectedActors)
{
	FTatiEditorStyle::InitalizeIcons();
	TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());

	if (SelectedActors.Num() > 0)
	{
		DebugHeader::PrintLog(TEXT("CustomLevelEditorMenuExtender") + FString::FromInt(SelectedActors.Num()));

		MenuExtender->AddMenuExtension(FName("ActorOptions"),
		                               EExtensionHook::Before,
		                               UICommandList,
		                               FMenuExtensionDelegate::CreateRaw(this, &FTatiEditorModule::AddLevelMenuEntry));
	}
	return MenuExtender;
}

void FTatiEditorModule::AddLevelMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Lock Actor Selection")),
		FText::FromString(TEXT("액터 선택하지 못하게 함")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "LevelEditor.SelectionLock"),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnLockActorSelectionButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("UnLock Actor Selection")),
		FText::FromString(TEXT("액터 선택 잠금 해제")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "LevelEditor.SelectionUnlock"),
		FExecuteAction::CreateRaw(this, &FTatiEditorModule::OnUnlockActorSelectionButtonClicked)
	);
}

void FTatiEditorModule::OnLockActorSelectionButtonClicked()
{
	if (GetEditorActorSubsystem() == false)
		return;

	TArray<AActor*> SelectedActors = WeakEditorActorSubsytem->GetSelectedLevelActors();
	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No Actors Selected"));
		return;
	}

	FString CurrentLockedActorNames = TEXT("Locked selection for : ");
	for (AActor* SelectedActor : SelectedActors)
	{
		if (SelectedActor == nullptr)
			continue;

		LockActorSelection(SelectedActor);
		WeakEditorActorSubsytem->SetActorSelectionState(SelectedActor, false);

		CurrentLockedActorNames.Append(TEXT("\n"));
		CurrentLockedActorNames.Append(SelectedActor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(CurrentLockedActorNames);
}

void FTatiEditorModule::OnUnlockActorSelectionButtonClicked()
{
	if (GetEditorActorSubsystem() == false)
		return;

	TArray<AActor*> AllActorsInLevel = WeakEditorActorSubsytem->GetAllLevelActors();
	TArray<AActor*> AllLockedActors;

	for (AActor* ActorInLevel : AllActorsInLevel)
	{
		if (ActorInLevel == nullptr)
			continue;

		if (CheckIsActorSelectionLocked(ActorInLevel))
		{
			AllLockedActors.Add(ActorInLevel);
		}
	}

	if (AllLockedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("현재 잠겨있는 액터 없음"));
	}

	FString UnlockedActorNames = TEXT("Unlokced Actors : ");
	for (AActor* LockedActor : AllLockedActors)
	{
		UnlockActorSelection(LockedActor);
		UnlockedActorNames.Append("\n");
		UnlockedActorNames.Append(LockedActor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(UnlockedActorNames);
}


#pragma  region SelectionLock
void FTatiEditorModule::InitCustomSelectionEvent()
{
	USelection* UserSelection = GEditor->GetSelectedActors();
	UserSelection->SelectObjectEvent.AddRaw(this, &FTatiEditorModule::OnActorSelected);
}

void FTatiEditorModule::OnActorSelected(UObject* SelectedObject)
{
	if (GetEditorActorSubsystem() == false)
		return;

	if (AActor* SelectedActor = Cast<AActor>(SelectedObject); SelectedActor != nullptr)
	{
		// 선택 잠금된 액터이면 선택 취소
		if (CheckIsActorSelectionLocked(SelectedActor))
		{
			WeakEditorActorSubsytem->SetActorSelectionState(SelectedActor, false);
		}
	}
}

void FTatiEditorModule::LockActorSelection(AActor* TargetActor)
{
	if (TargetActor == nullptr)
		return;

	if (TargetActor->ActorHasTag(FName("Locked")) == false)
	{
		TargetActor->Tags.Add(FName("Locked"));
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

#pragma  endregion


void FTatiEditorModule::RegisterAdvanceDeletionTab()
{
	DebugHeader::PrintLog(TEXT("RegisterAdvanceDeletionTab"));
	// 사용자 설정 탭 등록
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AdvanceDeletionName,
	                                                  FOnSpawnTab::CreateRaw(this, &FTatiEditorModule::OnSpawnDeletionTab))
	                        .SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FTatiEditorModule::OnSpawnDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// SNew - 슬레이트 위젯 생성
	return SNew(SDockTab).TabRole(NomadTab)
	[
		SNew(SAdvanceDeletionTab)
		.AssetDatas(GetAllAssetDatasUnderSelectedFolder()) // 에셋 데이터를 SAdvanceDeletionTab 파라미터로 넘김 
	];
}

TArray<TSharedPtr<FAssetData>> FTatiEditorModule::GetAllAssetDatasUnderSelectedFolder()
{
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);
	TArray<TSharedPtr<FAssetData>> AssetDatas;

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
			continue;

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AssetDatas.Add(MakeShared<FAssetData>(Data));
	}

	return AssetDatas;
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


#pragma  endregion


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
