// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Item/ItemData.h"
#include "PlaygroundGameMode.generated.h"

UCLASS(minimalapi)
class APlaygroundGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	APlaygroundGameMode();

	UPROPERTY(EditDefaultsOnly, Category=Item, meta=(AllowPrivateAccess=true))
	class UDroppedItemTable* DroppedItemTable;

};
