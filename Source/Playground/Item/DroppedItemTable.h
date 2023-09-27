// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DroppedItemTable.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FDroppingData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	class UItemData* ItemData;
	
	UPROPERTY(EditAnywhere)
	float AppearanceWeight = 1;

	UPROPERTY(EditAnywhere)
	int32 MinCnt = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxCnt = 1;
};

UCLASS()
class PLAYGROUND_API UDroppedItemTable : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FDroppingData> Table;

public:
	TPair<UItemData*, struct FItemStatus> GetDroppedItemData();
};
