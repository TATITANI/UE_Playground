// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TATIEDITOR_API FLevelEditorMenuExtension
{
public:
	FLevelEditorMenuExtension();
	~FLevelEditorMenuExtension();

private:
	void InitLevelEditorExtension();

	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList,
													const TArray<AActor*> SelectedActors);

	void AddLevelMenuEntry(FMenuBuilder& MenuBuilder);
	void OnLockActorSelectionButtonClicked();
	void OnUnlockActorSelectionButtonClicked();


	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);


#pragma  region CustomEditorUICommands

	TSharedPtr<class FUICommandList> CustomUICommands;
	void InitCustomUICommands();
#pragma  endregion

	
};
