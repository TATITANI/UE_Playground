// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "QuickMaterialCreationWidget.generated.h"

UENUM()
enum class EChannelPackingType : uint8
{
	NoChannelPacking UMETA(DisplayName="No Channel Packing") ,
	ORM UMETA(DisplayName="OcclusionRoughnessMetalic") ,
	MAX UMETA(DisplayName="MAX") ,
	
	
};

UCLASS()
class TATIEDITOR_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
#pragma  region Core

	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTexture();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromSelectedTextures", meta=( EditCondition= bCustomMaterialName))
	FString CreatingMaterialName = TEXT("M_");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromSelectedTextures")
	EChannelPackingType ChannelPackingType = EChannelPackingType::NoChannelPacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromSelectedTextures")
	bool bCreateMaterialInstance = false;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "CreateMaterialFromSelectedTextures")
	bool bCustomMaterialName = true;

#pragma  endregion


#pragma  region SupportedTextureNames

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> BaseColorArray = {
		TEXT("_BaseColor"),
		TEXT("_Albedo"),
		TEXT("_Diffuse"),
		TEXT("_diff")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> ORMArray = {
		TEXT("_arm"),
		TEXT("_ORM"),
		TEXT("_OcclusionRoughnessMetalic")
	};

#pragma  endregion

private:
#pragma  region Creation
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcess,
	                         TArray<UTexture2D*>& OutSelectedTextures, FString& OutSelectedTexturePackagePath);

	bool CheckIsNameUsed(const FString& FolderPath, const FString& AssetName);

	UMaterial* CreateMaterialAsset(const FString& MaterialName, const FString& CreatingPath);
	void CreateMaterialNodesDefault(UMaterial* Material, UTexture2D* SelectedTex, uint32& PinsConnectedCounter);
	void CreateMaterialNodesORM(UMaterial* Material, UTexture2D* SelectedTex, uint32& PinsConnectedCounter);


#pragma  endregion

#pragma region CreateMaterialNodes
	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode,
		UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);

	bool TryConnectMetalic(UMaterialExpressionTextureSample* TextureSampleNode,
		UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode,
		UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);

    bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode,
		UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);

	bool TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode,
		UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);

	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode,
			UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);

#pragma  endregion

	class UMaterialInstanceConstant* CreateMaterialInstanceAsset(UMaterial* CreatedMaterial,
		 FString MIName, const FString& PathToPut);

};
