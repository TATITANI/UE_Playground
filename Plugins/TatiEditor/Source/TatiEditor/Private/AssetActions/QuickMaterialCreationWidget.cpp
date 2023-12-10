// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"

#pragma  region Core
void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTexture()
{
	if (bCustomMaterialName)
	{
		if (CreatingMaterialName.IsEmpty() || CreatingMaterialName.Equals(TEXT("M_")))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please Enter a valid name"));
			return;
		}
	}

	TArray<FAssetData> SelectedAssetDats = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTextures;
	FString SelectedTextureFolderPath;

	if (ProcessSelectedData(SelectedAssetDats, SelectedTextures, SelectedTextureFolderPath) == false)
	{
		CreatingMaterialName = TEXT("M_");
		return;
	}

	if (CheckIsNameUsed(SelectedTextureFolderPath, CreatingMaterialName))
	{
		CreatingMaterialName = TEXT("M_");
		return;
	}


	UMaterial* CreatedMaterial = CreateMaterialAsset(CreatingMaterialName, SelectedTextureFolderPath);
	if (CreatedMaterial == nullptr)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to craete material"));
		return;
	}

	uint32 PinsConnectedCounter = 0;
	for (UTexture2D* SelectedTexture : SelectedTextures)
	{
		if (SelectedTexture == nullptr)
			continue;


		switch (ChannelPackingType)
		{
		case EChannelPackingType::NoChannelPacking:

			CreateMaterialNodesDefault(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
			break;
		case EChannelPackingType::ORM:
			CreateMaterialNodesORM(CreatedMaterial, SelectedTexture, PinsConnectedCounter);

			break;
		case EChannelPackingType::MAX:
			break;
		}
	}

	if (PinsConnectedCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Successfully connected %d pins")));
	}


	if (bCreateMaterialInstance)
	{
		CreateMaterialInstanceAsset(CreatedMaterial, CreatingMaterialName, SelectedTextureFolderPath);
	}

	CreatingMaterialName = TEXT("_M");

}

/**
 * @brief
 * Process the selected data, will filter out textures, and return false if non-texture selected
 */
bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcess, TArray<UTexture2D*>& OutSelectedTextures,
                                                       FString& OutSelectedTexturePackagePath)
{
	if (SelectedDataToProcess.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No texture selected"));
		return false;
	}

	bool bMaterialNameSet = false;

	for (const FAssetData& SelectedData : SelectedDataToProcess)
	{
		UObject* SelectedAsset = SelectedData.GetAsset();
		if (!SelectedAsset)
			continue;;

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);
		if (!SelectedTexture)
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,
			                           FString::Printf(TEXT("please select only textures : %s  is not a texture"), *SelectedAsset->GetName()));

			return false;
		}

		OutSelectedTextures.Add(SelectedTexture);

		if (OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = SelectedData.PackagePath.ToString();
		}

		if (!bCustomMaterialName && !bMaterialNameSet)
		{
			CreatingMaterialName = SelectedAsset->GetName();
			CreatingMaterialName.RemoveFromStart(TEXT("T_"));
			CreatingMaterialName.InsertAt(0, TEXT("M_"));

			bMaterialNameSet = true;
		}
	}

	return true;
}

bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPath, const FString& AssetName)
{
	TArray<FString> AsseetPaths = UEditorAssetLibrary::ListAssets(FolderPath, false);

	for (const FString& AssetPath : AsseetPaths)
	{
		// 확장자를 뺀 파일명
		const FString ExistingAssetName = FPaths::GetBaseFilename(AssetPath);

		if (ExistingAssetName.Equals(AssetName))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,
			                           FString::Printf(TEXT("%s is already used"), *AssetName));
			return true;
		}
	}


	return false;
}

UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& MaterialName, const FString& CreatingPath)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(MaterialName, CreatingPath, UMaterial::StaticClass(), MaterialFactory);

	return Cast<UMaterial>(CreatedObject);
}

void UQuickMaterialCreationWidget::CreateMaterialNodesDefault(UMaterial* Material, UTexture2D* SelectedTex, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(Material);

	if (TextureSampleNode == nullptr)
		return;

	if (Material->HasBaseColorConnected() == false)
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasMetallicConnected() == false)
	{
		if (TryConnectMetalic(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasRoughnessConnected() == false)
	{
		if (TryConnectRoughness(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasNormalConnected() == false)
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasAmbientOcclusionConnected() == false)
	{
		if (TryConnectAO(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}
}

void UQuickMaterialCreationWidget::CreateMaterialNodesORM(UMaterial* Material, UTexture2D* SelectedTex, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(Material);

	if (TextureSampleNode == nullptr)
		return;

	if (Material->HasBaseColorConnected() == false)
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasNormalConnected() == false)
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (Material->HasRoughnessConnected() == false)
	{
		if (TryConnectORM(TextureSampleNode, SelectedTex, Material))
		{
			PinsConnectedCounter++;
			return;
		}
	}
}

#pragma region CreateMaterialNodes

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                       UMaterial* CreatedMaterial)
{
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			// connect pins to base color
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->MaterialExpressionEditorX -= 600; // 위치 조정

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->BaseColor.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectMetalic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                     UMaterial* CreatedMaterial)
{
	for (const FString& MetalicName : MetallicArray)
	{
		if (SelectedTexture->GetName().Contains(MetalicName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
			TextureSampleNode->MaterialExpressionEditorX -= 600; // 위치 조정
			TextureSampleNode->MaterialExpressionEditorY += 250; // 위치 조정

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Metallic.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();
			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                       UMaterial* CreatedMaterial)
{
	for (const FString& RoughnessName : RoughnessArray)
	{
		if (SelectedTexture->GetName().Contains(RoughnessName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
			TextureSampleNode->MaterialExpressionEditorX -= 600; // 위치 조정
			TextureSampleNode->MaterialExpressionEditorY += 500; // 위치 조정

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Roughness.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();
			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                    UMaterial* CreatedMaterial)
{
	for (const FString& NormalName : NormalArray)
	{
		if (SelectedTexture->GetName().Contains(NormalName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Normal;

			TextureSampleNode->MaterialExpressionEditorX -= 600; // 위치 조정
			TextureSampleNode->MaterialExpressionEditorY += 750; // 위치 조정

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Normal.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                UMaterial* CreatedMaterial)
{
	for (const FString& AOName : AmbientOcclusionArray)
	{
		if (SelectedTexture->GetName().Contains(AOName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 1000;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture,
                                                 UMaterial* CreatedMaterial)
{
	for (const FString& ORMName : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ORMName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Masks;

			CreatedMaterial->GetExpressionCollection().Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Connect(1, TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Roughness.Connect(2, TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Metallic.Connect(3, TextureSampleNode);
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 1000;

			return true;
		}
	}

	return false;
}

UMaterialInstanceConstant* UQuickMaterialCreationWidget::CreateMaterialInstanceAsset(UMaterial* CreatedMaterial, FString MIName,
                                                                                     const FString& PathToPut)
{
	MIName.RemoveFromStart(TEXT("M_"));
	MIName.InsertAt(0,TEXT("MI_"));

	UMaterialInstanceConstantFactoryNew* MIFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(MIName,PathToPut,
	UMaterialInstanceConstant::StaticClass(),MIFactoryNew);
	
	if(UMaterialInstanceConstant* CreatedMI = Cast<UMaterialInstanceConstant>(CreatedObject))
	{
		CreatedMI->SetParentEditorOnly(CreatedMaterial);
	
		CreatedMI->PostEditChange();
		CreatedMaterial->PostEditChange();
	
		return CreatedMI;
	}

	return nullptr;
}


#pragma  endregion

#pragma  endregion
