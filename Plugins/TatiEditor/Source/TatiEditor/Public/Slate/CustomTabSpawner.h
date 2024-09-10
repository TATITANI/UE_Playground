// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Docking/SDockTab.h"
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
	TSharedRef<SDockTab> OnSpawnTab(const FSpawnTabArgs& SpawnTabArgs, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab);
};


template <class T>
void FCustomTabSpawner::RegisterCustomTab(FName TabID, FOnConstructSlate OnConstructSlate, FOnCloseTab OnCloseTab)
{
	// 사용자 설정 탭 등록
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        TabID, FOnSpawnTab::CreateRaw(this, &FCustomTabSpawner::OnSpawnTab, OnConstructSlate, OnCloseTab))
	                        .SetDisplayName(FText::FromString(FString::Printf(TEXT("Open %s"), *TabID.ToString())));
}
