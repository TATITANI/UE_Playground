// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/CBMenuExtension.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "TatiEditor.h"
#include "CustomStyle/TatiEditorStyle.h"
#include "Slate/AdvanceDeletionWidget.h"


FCBMenuExtension::FCBMenuExtension()
{
	Init();
}

FCBMenuExtension::~FCBMenuExtension()
{
}

void FCBMenuExtension::Init()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	// 콘텐츠 브라우저 컨텍스트메뉴 
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// 컨텍스트 메뉴가 열리면 호출됨
	CBModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FCBMenuExtension::CustomCBMenuExtender));

	
}


TSharedRef<FExtender> FCBMenuExtension::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
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
		                               FMenuExtensionDelegate::CreateRaw(this, &FCBMenuExtension::AddCBMenuEntry));
	}


	return MenuExtender;
}


void FCBMenuExtension::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all unused assets under folder")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"),
		FExecuteAction::CreateRaw(this, &FCBMenuExtension::OnDeleteUnusedAssetButtonClicked)
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete all empty folders")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FCBMenuExtension::OnDeleteEmtpyFoldersButtonClicked)

	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advance Delete")),
		FText::FromString(TEXT("에셋 상태별 리스트")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"),
		FExecuteAction::CreateRaw(this, &FCBMenuExtension::OnAdvanceDeleteButtonClicked)

	);
}

void FCBMenuExtension::OnDeleteUnusedAssetButtonClicked()
{
	// advance deletionTab이 열린채로 에셋을 삭제하면, 탭에는 삭제한 내용이 반영되지 않기 때문에 생기는 문제를 차단 
	if(FTatiEditorModule::ConstructedDockTab.IsValid())
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please Close AdvanceDeletionTab"));
		return;
	}
	
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

void FCBMenuExtension::OnDeleteEmtpyFoldersButtonClicked()
{
	if(FTatiEditorModule::ConstructedDockTab.IsValid())
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please Close AdvanceDeletionTab"));
		return;
	}
	
	FTatiEditorModule::FixUpRedirectors();

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

void FCBMenuExtension::OnAdvanceDeleteButtonClicked()
{
	FTatiEditorModule::FixUpRedirectors();

	FGlobalTabmanager::Get()->TryInvokeTab( SAdvanceDeletionTab::AdvanceDeletionName);
	DebugHeader::PrintLog(TEXT("OnAdvanceDeleteButtonClicked"));
}


TArray<TSharedPtr<FAssetData>> FCBMenuExtension::GetAllAssetDatasUnderSelectedFolder()
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
