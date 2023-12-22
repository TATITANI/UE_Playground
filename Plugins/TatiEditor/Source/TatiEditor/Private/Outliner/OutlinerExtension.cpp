// Fill out your copyright notice in the Description page of Project Settings.


#include "Outliner/OutlinerExtension.h"

#include "DebugHeader.h"
#include "SceneOutlinerModule.h"
#include "TatiEditor.h"
#include "Outliner/OutlinerSelectionColumn.h"
#include "Subsystems/EditorActorSubsystem.h"

FOutlinerExtension::FOutlinerExtension()
{
	RegisterOutlinerColumn();
}

FOutlinerExtension::~FOutlinerExtension()
{
	UnregisterOutlinerColumExtension();

}

void FOutlinerExtension::RegisterOutlinerColumn()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	FSceneOutlinerColumnInfo SelectionLockColumnInfo(ESceneOutlinerColumnVisibility::Visible, 1,
	                                                 FCreateSceneOutlinerColumn::CreateRaw(this,
	                                                                                       &FOutlinerExtension::OnCreateSelectionLockColumn));

	SceneOutlinerModule.RegisterDefaultColumnType<FOutlinerSelectionColumn>(SelectionLockColumnInfo);
}


TSharedRef<ISceneOutlinerColumn> FOutlinerExtension::OnCreateSelectionLockColumn(ISceneOutliner& SceneOutliner)
{
	return MakeShareable(new FOutlinerSelectionColumn(SceneOutliner));
}

void FOutlinerExtension::UnregisterOutlinerColumExtension()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	SceneOutlinerModule.UnRegisterColumnType<FOutlinerSelectionColumn>();
}


void FOutlinerExtension::ProcessLockingOutliner(AActor* ActorToProcess, bool bShoudLock)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));
	
	if (!TatiEditorModule.GetEditorActorSubsystem())
		return;

	if (bShoudLock)
	{
		TatiEditorModule.LockActorSelection(ActorToProcess);
		TatiEditorModule.WeakEditorActorSubsytem->SetActorSelectionState(ActorToProcess, false);
		DebugHeader::ShowNotifyInfo(TEXT("Locked selection for : \n") + ActorToProcess->GetActorLabel());
	}
	else
	{
		TatiEditorModule.UnlockActorSelection(ActorToProcess);
		DebugHeader::ShowNotifyInfo(TEXT("Removed selection for : \n") + ActorToProcess->GetActorLabel());
	}
}
