// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaygroundGameMode.h"

#include "Item/DroppedItemTable.h"
#include "Item/ItemData.h"
#include "UI/IngameWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Utils/UtilPlayground.h"

APlaygroundGameMode::APlaygroundGameMode()
	: Super()
{
}

void APlaygroundGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PG_LOG(LogPGNetwork, Log, TEXT("%s logIn"), *NewPlayer->GetName());
}

void APlaygroundGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	PG_LOG(LogPGNetwork, Log, TEXT("%s logOut"), *Exiting->GetName());

}
