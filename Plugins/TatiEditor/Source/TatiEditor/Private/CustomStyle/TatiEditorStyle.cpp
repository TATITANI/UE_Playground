// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/TatiEditorStyle.h"

#include "DebugHeader.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FTatiEditorStyle::StyleSetName = FName("TatiEditorStyle");
TSharedPtr<FSlateStyleSet> FTatiEditorStyle::CreatedSlateStyleSet = nullptr;

void FTatiEditorStyle::InitalizeIcons()
{
	if (CreatedSlateStyleSet.IsValid() == false)
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

void FTatiEditorStyle::Shutdown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset(); // removing a reference to the objects
	}
}

TSharedRef<FSlateStyleSet> FTatiEditorStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShared<FSlateStyleSet>(StyleSetName);

	// note : IPluginManager 가 속한 Projects 모듈을 build.cs에 추가해야함
	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("TatiEditor"))->GetBaseDir()/"Resources";
	DebugHeader::PrintLog(FString::Printf(TEXT("icon directory : %s"),*IconDirectory));
	CustomStyleSet->SetContentRoot(IconDirectory);
	
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
	                    new FSlateImageBrush(IconDirectory/"DeleteUnusedAsset.png", CoreStyleConstants::Icon16x16));

	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
						new FSlateImageBrush(IconDirectory/"DeleteEmptyFolders.png", CoreStyleConstants::Icon16x16));

	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
						new FSlateImageBrush(IconDirectory/"AdvanceDeletion.png", CoreStyleConstants::Icon16x16));

	CustomStyleSet->Set("LevelEditor.SelectionLock",
						new FSlateImageBrush(IconDirectory/"SelectionLock.png", CoreStyleConstants::Icon16x16));

	CustomStyleSet->Set("LevelEditor.SelectionUnlock",
						new FSlateImageBrush(IconDirectory/"SelectionUnlock.png", CoreStyleConstants::Icon16x16));
	

	return CustomStyleSet;
}
