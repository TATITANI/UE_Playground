// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType, Atomic)
struct FItemStatus
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Count = 0;
};


UCLASS()
class PLAYGROUND_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemData() : MaxCount(99999)
	{
	}
	
	/** Type of this item, set in native parent class */
	UPROPERTY(EditAnywhere, Category = Item)
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TSubclassOf<class APlaygroundItem> DroppedItem;
	
	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	UTexture2D *ItemIcon;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 MaxCount;
	
	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;
	

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
