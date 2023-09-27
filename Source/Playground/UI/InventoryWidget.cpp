// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InventorySlotWidget.h"
#include "MyGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Hidden);
	AssignInput();
}


void UInventoryWidget::AssignInput()
{
	const auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	ensure(Subsystem != nullptr);
	if (Subsystem->HasMappingContext(InventoryMappingContext) == false)
	{
		Subsystem->AddMappingContext(InventoryMappingContext, PlayerController->InputPriority + 1);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	EnhancedInputComponent->BindAction(ToggleInputAction, ETriggerEvent::Started, this, &UInventoryWidget::ToggleOpenClose);
}


void UInventoryWidget::Open()
{
	SetVisibility(ESlateVisibility::Visible);
	GetOwningPlayer()->SetShowMouseCursor(true);

	UpdateInfo();
	bOpen = true;
}

void UInventoryWidget::UpdateInfo()
{
	auto Slots = Grid_Slot->GetAllChildren();
	auto ItemInventory = Cast<UMyGameInstance>(GetGameInstance())->ItemInventory;
	auto InventoryDatas = ItemInventory->GetTable().Array();
	for (int32 SlotID = 0; SlotID < Slots.Num(); SlotID++)
	{
		const auto SlotWidget = Cast<UInventorySlotWidget>(Slots[SlotID]);
		if (bool IsItemSlot = SlotID < InventoryDatas.Num())
		{
			const auto ItemData = InventoryDatas[SlotID].Key;
			const auto Cnt = InventoryDatas[SlotID].Value.Count;
			SlotWidget->SetActive(true);
			SlotWidget->UpdateData(ItemData, Cnt);

			if (SlotID == SelectedSlotID)
			{
				TXT_ItemName->SetText(ItemData->ItemName);
			}
		}
		else
		{
			SlotWidget->SetActive(false);
		}
	}

	if (InventoryDatas.Num() > Slots.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("more item data than slots"));
	}
}

void UInventoryWidget::Close()
{
	GetOwningPlayer()->SetShowMouseCursor(false);
	SetVisibility(ESlateVisibility::Hidden);
	bOpen = false;
}

void UInventoryWidget::ToggleOpenClose()
{
	if (bOpen)
		Close();
	else
		Open();
}
