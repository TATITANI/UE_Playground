// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/**
 * 
 */
class FTatiUICommands : public TCommands<FTatiUICommands>
{
public:
	FTatiUICommands() : TCommands<FTatiUICommands>(TEXT("TatiEditor"),
		FText::FromString(TEXT("Tati Editor UI Commands")),
			NAME_None,TEXT("TatiEditor")){}
	
	~FTatiUICommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;
};
