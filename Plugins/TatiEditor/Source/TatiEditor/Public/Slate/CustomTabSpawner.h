// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TATIEDITOR_API FCustomTabSpawner
{
public:
	FCustomTabSpawner();
	~FCustomTabSpawner();

public:
	DECLARE_DELEGATE_RetVal(TSharedRef<SCompoundWidget>, FOnConstructSlate);
	DECLARE_DELEGATE(FOnCloseTab)

	template <class T>
	void RegisterCustomTab(FName TabID, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab);

private:
	template <class T>
	TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab);
};


template <class T>
void FCustomTabSpawner::RegisterCustomTab(FName TabID, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab)
{
	// 사용자 설정 탭 등록
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        TabID, FOnSpawnTab::CreateRaw(this, &FCustomTabSpawner::OnSpawnTab<T>, OnConstructSlate, OnCloseTab))
	                        .SetDisplayName(FText::FromString(FString::Printf(TEXT("%s Open"), *TabID.ToString())));
}

template <class T>
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
