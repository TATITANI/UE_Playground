// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemInventory.generated.h"

/**
 * 
 */
UCLASS()
class UItemInventory : public UObject 
{
	GENERATED_BODY()
public:
	UItemInventory();

private:
	UPROPERTY(VisibleAnywhere)
	TMap<UItemData*, FItemStatus> Table;

public:
	void AddItem(UItemData* ItemData, int32 Cnt);
	
	TOptional<FItemStatus> GetItem(UItemData* ItemData);

	UFUNCTION(BlueprintCallable)
	TMap<UItemData*, FItemStatus> GetTable();
};
