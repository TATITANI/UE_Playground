// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/CustomTabSpawner.h"

#include "TatiEditor.h"

FCustomTabSpawner::FCustomTabSpawner()
{
}

FCustomTabSpawner::~FCustomTabSpawner()
{
}

TSharedRef<SDockTab> FCustomTabSpawner::OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab)
{
	FTatiEditorModule& TatiEditorModule = FModuleManager::LoadModuleChecked<FTatiEditorModule>(TEXT("TatiEditor"));

	// SNew - 슬레이트 위젯 생성
	TatiEditorModule.ConstructedDockTab = SNew(SDockTab).TabRole(NomadTab)
	[
		OnConstructSlate.Execute()
	];

	TatiEditorModule.ConstructedDockTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([&](TSharedRef<SDockTab>)
	{
		if (TatiEditorModule.ConstructedDockTab.IsValid())
		{
			TatiEditorModule.ConstructedDockTab.Reset();
			OnCloseTab.ExecuteIfBound();
		}
	}));


	return TatiEditorModule.ConstructedDockTab.ToSharedRef();
}
