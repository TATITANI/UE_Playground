// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/LevelEditorMenuExtension.h"

#include "DebugHeader.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "TatiEditor.h"
#include "CustomStyle/TatiEditorStyle.h"
#include "CustomUICommand/TatiUICommands.h"
#include "Subsystems/EditorActorSubsystem.h"

FLevelEditorMenuExtension::FLevelEditorMenuExtension()
{
	InitCustomUICommands();
	InitCustomSelectionEvent();
	InitLevelEditorExtension();
}

FLevelEditorMenuExtension::~FLevelEditorMenuExtension()
{
}


void FLevelEditorMenuExtension::InitLevelEditorExtension()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	TSharedRef<FUICommandList> ExistingLevelCommands = LevelEditorModule.GetGlobalLevelEditorActions();
	ExistingLevelCommands->Append(CustomUICommands.ToSharedRef());

	TArray<FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors>& LevelEditorMenuExtenders =
		LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	LevelEditorMenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this,
		&FLevelEditorMenuExtension::CustomLevelEditorMenuExtender));
}


TSharedRef<FExtender> FLevelEditorMenuExtension::CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList,
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
									   FMenuExtensionDelegate::CreateRaw(this, &FLevelEditorMenuExtension::AddLevelMenuEntry));
	}
	return MenuExtender;
}



void FLevelEditorMenuExtension::AddLevelMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Lock Actor Selection")),
		FText::FromString(TEXT("액터 선택하지 못하게 함")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "LevelEditor.SelectionLock"),
		FExecuteAction::CreateRaw(this, &FLevelEditorMenuExtension::OnLockActorSelectionButtonClicked)
	);

	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("UnLock Actor Selection")),
		FText::FromString(TEXT("액터 선택 잠금 해제")),
		FSlateIcon(FTatiEditorStyle::GetStyleSetName(), "LevelEditor.SelectionUnlock"),
		FExecuteAction::CreateRaw(this, &FLevelEditorMenuExtension::OnUnlockActorSelectionButtonClicked)
	);
}

void FLevelEditorMenuExtension::OnLockActorSelectionButtonClicked()
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	
	if (TatiEditorModule.GetEditorActorSubsystem() == false)
		return;

	TArray<AActor*> SelectedActors = TatiEditorModule.WeakEditorActorSubsytem->GetSelectedLevelActors();
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

		TatiEditorModule.LockActorSelection(SelectedActor);
		TatiEditorModule.WeakEditorActorSubsytem->SetActorSelectionState(SelectedActor, false);

		CurrentLockedActorNames.Append(TEXT("\n"));
		CurrentLockedActorNames.Append(SelectedActor->GetActorLabel());
	}

	TatiEditorModule.RefreshSceneOutliner();

	DebugHeader::ShowNotifyInfo(CurrentLockedActorNames);
}

void FLevelEditorMenuExtension::OnUnlockActorSelectionButtonClicked()
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	if (TatiEditorModule.GetEditorActorSubsystem() == false)
		return;

	TArray<AActor*> AllActorsInLevel = TatiEditorModule.WeakEditorActorSubsytem->GetAllLevelActors();
	TArray<AActor*> AllLockedActors;

	for (AActor* ActorInLevel : AllActorsInLevel)
	{
		if (ActorInLevel == nullptr)
			continue;

		if (TatiEditorModule.CheckIsActorSelectionLocked(ActorInLevel))
		{
			AllLockedActors.Add(ActorInLevel);
		}
	}

	if (AllLockedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("현재 잠겨있는 액터 없음"));
		return;
	}

	FString UnlockedActorNames = TEXT("Unlokced Actors : ");
	for (AActor* LockedActor : AllLockedActors)
	{
		TatiEditorModule.UnlockActorSelection(LockedActor);
		UnlockedActorNames.Append("\n");
		UnlockedActorNames.Append(LockedActor->GetActorLabel());
	}

	TatiEditorModule.RefreshSceneOutliner();

	DebugHeader::ShowNotifyInfo(UnlockedActorNames);
}


#pragma  region CustomEditorUICommands

void FLevelEditorMenuExtension::InitCustomUICommands()
{
	CustomUICommands = MakeShareable(new FUICommandList());

	CustomUICommands->MapAction(
		FTatiUICommands::Get().LockActorSelection,
		FExecuteAction::CreateRaw(this, &FLevelEditorMenuExtension::OnLockActorSelectionButtonClicked)
	);

	CustomUICommands->MapAction(
		FTatiUICommands::Get().UnlockActorSelection,
		FExecuteAction::CreateRaw(this, &FLevelEditorMenuExtension::OnUnlockActorSelectionButtonClicked)
	);
}

#pragma  endregion


void FLevelEditorMenuExtension::InitCustomSelectionEvent()
{
	USelection* UserSelection = GEditor->GetSelectedActors();
	UserSelection->SelectObjectEvent.AddRaw(this, &FLevelEditorMenuExtension::OnActorSelected);
}

void FLevelEditorMenuExtension::OnActorSelected(UObject* SelectedObject)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	if (TatiEditorModule.GetEditorActorSubsystem() == false)
		return;

	if (AActor* SelectedActor = Cast<AActor>(SelectedObject); SelectedActor != nullptr)
	{
		// 선택 잠금된 액터이면 선택 취소
		if (TatiEditorModule.CheckIsActorSelectionLocked(SelectedActor))
		{
			TatiEditorModule.WeakEditorActorSubsytem->SetActorSelectionState(SelectedActor, false);
		}
	}
}

