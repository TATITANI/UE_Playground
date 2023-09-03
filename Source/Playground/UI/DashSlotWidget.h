// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UDashSlotWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	void ActiveCooldown(double _CoolBeginSeconds, double _CoolEndSeconds);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UMaterialInstanceDynamic* MaterialInstanceDynamic;

	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UImage* Img_Panel;

	bool IsCooldown = false;
	double CoolBeginSeconds, CoolEndSeconds;
	const FName KeyMaterialProgress = "Progress";
	
};
