// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyHUD.h"
#include "UI/IngameWidget.h"
void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	IngameWidget = Cast<UIngameWidget>(CreateWidget(GetWorld(), IngameWidgetSubclassOf));
	IngameWidget->AddToViewport();
}
