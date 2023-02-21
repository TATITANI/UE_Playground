// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UMyHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget)) // c++ 에서 widget 블루프린트 접근
	class UTextBlock* AmmoText; // 에디터 상 컴포넌트 이름과 같아야 함.
};
