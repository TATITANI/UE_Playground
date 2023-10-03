// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Item/ItemData.h"
#include "PlaygroundSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UPlaygroundSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TMap<UItemData*, struct FItemStatus> ItemTable;

};
