// Fill out your copyright notice in the Description page of Project Settings.


#include "Outliner/OutlinerExtension.h"

#include "DebugHeader.h"
#include "SceneOutlinerModule.h"
#include "TatiEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Outliner/OutlinerActorActivationColumn.h"
#include "Outliner/OutlinerSelectionLockColumn.h"
#include "Subsystems/EditorActorSubsystem.h"

FOutlinerExtension::FOutlinerExtension()
{
	RegisterOutlinerColumn();
	
	// todo : level change 시에 세이브 로드
	// FWorldDelegates::LevelAddedToWorld
}

FOutlinerExtension::~FOutlinerExtension()
{
	UnregisterOutlinerColumExtension();

}


FString FOutlinerExtension::GetSaveKey(FString LevelName)
{
	FString Key;
	return Key;
}

void FOutlinerExtension::InitSave()
{
	DebugHeader::PrintLog(TEXT("FTatiEditorModule::InitSave"));
	SavingData = Cast<UTatiEditorOutlinerSave>(UGameplayStatics::LoadGameFromSlot(TEXT("TatiOutliner"), 0));

	if (SavingData->IsValidLowLevel() == false)
	{
		DebugHeader::PrintLog(TEXT("SavingData invalid"));
		return;
	}

	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	ensureMsgf(TatiEditorModule.GetEditorActorSubsystem() == true, TEXT("InitSave - EditorActorSubsytem invalid"));
	
	auto AllActors = TatiEditorModule.WeakEditorActorSubsytem->GetAllLevelActors();
	DebugHeader::PrintLog(FString::Printf(TEXT("AllActors num in tatiEditor :  %d"), AllActors.Num()));

	// todo
	// SavingData있는 액터이면 활성화 설정
	
}


void FOutlinerExtension::RegisterOutlinerColumn()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));
	FSceneOutlinerColumnInfo SelectionLockColumnInfo(ESceneOutlinerColumnVisibility::Visible, 1,
	                                                 FCreateSceneOutlinerColumn::CreateRaw(this,
	                                                                                       &FOutlinerExtension::OnCreateSelectionLockColumn));
	
	SceneOutlinerModule.RegisterDefaultColumnType<FOutlinerSelectionLockColumn>(SelectionLockColumnInfo);

	
	// 액터 활성화 토글은 서브레벨로 대체 가능해서 구현 않기로. 다른 아이디어 있으면 추후에 적용 
	// FSceneOutlinerColumnInfo ActivationColumnInfo(ESceneOutlinerColumnVisibility::Visible, 1,
	//                                                  FCreateSceneOutlinerColumn::CreateRaw(this,
	//                                                                                        &FOutlinerExtension::OnCreateActivationColumn));
	//
	// SceneOutlinerModule.RegisterDefaultColumnType<FOutlinerActorActivationColumn>(ActivationColumnInfo);
}


TSharedRef<ISceneOutlinerColumn> FOutlinerExtension::OnCreateSelectionLockColumn(ISceneOutliner& SceneOutliner)
{
	return MakeShareable(new FOutlinerSelectionLockColumn(SceneOutliner));
}

TSharedRef<ISceneOutlinerColumn> FOutlinerExtension::OnCreateActivationColumn(ISceneOutliner& SceneOutliner)
{
	return MakeShareable(new FOutlinerActorActivationColumn(SceneOutliner));

}

void FOutlinerExtension::UnregisterOutlinerColumExtension()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>(TEXT("SceneOutliner"));

	SceneOutlinerModule.UnRegisterColumnType<FOutlinerSelectionLockColumn>();
	// SceneOutlinerModule.UnRegisterColumnType<FOutlinerActorActivationColumn>();
}

