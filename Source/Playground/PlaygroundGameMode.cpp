// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaygroundGameMode.h"

#include "UI/IngameWidget.h"
#include "UObject/ConstructorHelpers.h"

APlaygroundGameMode::APlaygroundGameMode()
	: Super()
{
}

void APlaygroundGameMode::BeginPlay()
{
	if (InGameWidget != nullptr)
		ChangeWidget(InGameWidget);
}

void APlaygroundGameMode::ChangeWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("ChangeWidget : %s"), *CurrentWidget->GetName());
		}
	}
}
