// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlaygroundGameMode.generated.h"

UCLASS(minimalapi)
class APlaygroundGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlaygroundGameMode();

public:

	UPROPERTY()
	TSubclassOf<UUserWidget> HUD_Class;
	
	UPROPERTY()
	UUserWidget* CurrentWidget;
};



