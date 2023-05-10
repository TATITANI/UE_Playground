// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaygroundGameMode.h"

#include "Character/Protagonist/ProtagonistCharacter.h"
#include "MyHUD.h"
#include "UObject/ConstructorHelpers.h"

APlaygroundGameMode::APlaygroundGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_ProtagonistCharacter.BP_ProtagonistCharacter'"));
	
	// DefaultPawnClass = PlayerPawnClassFinder.Class;
	//
	// HUDClass = AProtagonistCharacter::StaticClass();
	//
	// static ConstructorHelpers::FClassFinder<UMyHUD> UI_HUD(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/WBP_HUD.WBP_HUD_C'"));
	// if(UI_HUD.Succeeded())
	// {
	// 	HUD_Class = UI_HUD.Class;
	//
	// 	CurrentWidget = CreateWidget(GetWorld(), HUD_Class);
	// 	if(CurrentWidget)
	// 	{
	// 		CurrentWidget->AddToViewport();
	// 		// CurrentWidget->RemoveFromViewport();
	// 	}
	// }

}
