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
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess= true))
	TSubclassOf<UUserWidget> InGameWidget;

public:
	UPROPERTY()
	UUserWidget* CurrentWidget;

public:
	UFUNCTION()
	void ChangeWidget(TSubclassOf<UUserWidget> NewWidgetClass);
};
