// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TatiEditorOutlinerSave.generated.h"

/**
 * 
 */
UCLASS()
class TATIEDITOR_API UTatiEditorOutlinerSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TMap<uint32, bool> OutlinerActorActivationMap; // uniqueID, active

};
