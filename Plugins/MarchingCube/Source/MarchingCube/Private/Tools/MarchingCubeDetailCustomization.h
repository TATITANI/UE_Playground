// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"
#include "PreviewMesh.h"

/**
 * 
 */
class MARCHINGCUBE_API FMarchingCubeDetailCustomization : public IDetailCustomization
{
public:
	FMarchingCubeDetailCustomization();
	~FMarchingCubeDetailCustomization();

	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FReply OnClickGenerate();

};
