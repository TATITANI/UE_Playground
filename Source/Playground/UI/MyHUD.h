// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PLAYGROUND_API AMyHUD : public AHUD
{
	GENERATED_BODY()

private:
	AMyHUD();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category= Widget, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UIngameWidget> IngameWidgetClass;

	UPROPERTY(EditAnywhere, Category= Widget, meta=(AllowPrivateAccess=true))
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

public:
	UPROPERTY(BlueprintReadOnly)
	class UIngameWidget* IngameWidget;
	
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly)
	bool bHiddenInGameWidgetAtStart = false;
	
};
