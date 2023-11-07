// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "TatiEditor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/MessageDialog.h"

void UQuickAssetAction::TestFunc()
{
	DebugHeader::Print(TEXT("Working"), FColor::Cyan);
}

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::Print(TEXT("Please enter a VALID number"), FColor::Red);

		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter valid number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetSoftObjectPath().ToString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				Counter++;
			}
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Duplate Success : ") + FString::FromInt(Counter) + " files");
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)
			continue;

		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			DebugHeader::Print(TEXT("Failed to find prefix for class") + SelectedObject->GetClass()->GetName(), FColor::Red);
			continue;
		}
		FString OldName = SelectedObject->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			continue;
		}

		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}

		const FString NewName = *PrefixFound + OldName;
		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewName);

		Counter++;
	}

	DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Rename %d Assets"), Counter));
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	// FixUpRedirectors();
	FTatiEditorModule::FixUpRedirectors();

	for (auto SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> ReferencePath = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString());
		if (ReferencePath.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Unused Assets among selected assets"), false);
		return;
	}

	int32 DeleteCnt = ObjectTools::DeleteAssets(UnusedAssetsData);

	DebugHeader::ShowMsgDialog(EAppMsgType::Ok, FString::Printf(TEXT("Delete %d Assets"), DeleteCnt), false);
}
