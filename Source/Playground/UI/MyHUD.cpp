// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyHUD.h"

#include "InventoryWidget.h"
#include "UI/IngameWidget.h"

AMyHUD::AMyHUD()
{
}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	IngameWidget = Cast<UIngameWidget>(CreateWidget(GetWorld(), IngameWidgetClass));
	IngameWidget->AddToViewport();
	
	InventoryWidget = Cast<UInventoryWidget>(CreateWidget(GetWorld(), InventoryWidgetClass));
	InventoryWidget->AddToViewport();

	IngameWidget->SetVisibility(bHiddenInGameWidgetAtStart ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

