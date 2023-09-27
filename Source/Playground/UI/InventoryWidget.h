// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UUniformGridPanel* Grid_Slot;

	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UTextBlock* TXT_ItemName;
	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UTextBlock* TXT_ItemDescription;

	int SelectedSlotID = 0;

private:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class UInputMappingContext* InventoryMappingContext;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	class UInputAction* ToggleInputAction;

	bool bOpen = false;


	void UpdateInfo();
	void AssignInput();

protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable)
	void Close();
	
	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION(BlueprintCallable)
	void ToggleOpenClose();
};
