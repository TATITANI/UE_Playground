// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneOutliner/Private/SceneOutlinerActorSCCColumn.h"

class UTatiEditorOutlinerSave;
/**
 * 
 */

class TATIEDITOR_API FOutlinerExtension
{
public:
	FOutlinerExtension();
	~FOutlinerExtension();

public:
	UPROPERTY()
	UTatiEditorOutlinerSave* SavingData;

private:
	FString GetSaveKey(FString LevelName);
	// fstring GenerateSaveKey()

private:
	void InitSave();


	void RegisterOutlinerColumn();
	TSharedRef<class ISceneOutlinerColumn> OnCreateSelectionLockColumn(class ISceneOutliner& SceneOutliner);
	TSharedRef<class ISceneOutlinerColumn> OnCreateActivationColumn(class ISceneOutliner& SceneOutliner);
	void UnregisterOutlinerColumExtension();
};
