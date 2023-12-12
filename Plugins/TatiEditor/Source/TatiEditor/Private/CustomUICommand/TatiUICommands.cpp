// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUICommand/TatiUICommands.h"

#define LOCTEXT_NAMESPACE "FTatiEditorModule"


FTatiUICommands::~FTatiUICommands()
{
}

void FTatiUICommands::RegisterCommands()
{
	UI_COMMAND(LockActorSelection,
	           "Lock Actor Selection",
	           "액터 선택 방지",
	           EUserInterfaceActionType::Button,
	           FInputChord(EKeys::W, EModifierKey::Alt));

	UI_COMMAND(UnlockActorSelection,
		   "UnLock Actor Selection",
		   "액터 선택 방지 해제",
		   EUserInterfaceActionType::Button,
		   FInputChord(EKeys::W , EModifierKey::Alt | EModifierKey::Shift));
	
}

#undef LOCTEXT_NAMESPACE
