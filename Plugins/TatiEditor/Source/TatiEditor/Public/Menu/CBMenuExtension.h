// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/** Content Browser 메뉴
 * 
 */
class TATIEDITOR_API FCBMenuExtension
{
	
public:
	FCBMenuExtension();
	~FCBMenuExtension();
	
private:

	void Init();

	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetButtonClicked();

	void OnDeleteEmtpyFoldersButtonClicked();
	void OnAdvanceDeleteButtonClicked();

	
	

public:
	TArray<FString> FolderPathsSelected;

	TArray<TSharedPtr<FAssetData>> GetAllAssetDatasUnderSelectedFolder();
	
};
