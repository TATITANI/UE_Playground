// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InventorySlotWidget.h"
#include "MyGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Hidden);
	AssignInput();

	auto SlotsWidgets = Grid_Slot->GetAllChildren();
	for (int32 SlotID = 0; SlotID < SlotsWidgets.Num(); SlotID++)
	{
		auto ItemSlot = Cast<UInventorySlotWidget>(SlotsWidgets[SlotID]);
		ItemSlot->Init(this, SlotID);
		this->ItemSlots.Add(ItemSlot);
	}
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
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	SetVisibility(ESlateVisibility::Visible);
	GetOwningPlayer()->SetShowMouseCursor(true);

	UpdateInfo();
	bOpen = true;
}

void UInventoryWidget::UpdateInfo()
{
	auto ItemInventory = Cast<UMyGameInstance>(GetGameInstance())->ItemInventory->GetTable();

	for (int32 SlotID = 0; SlotID < ItemSlots.Num(); SlotID++)
	{
		const auto SlotWidget = ItemSlots[SlotID];
		SlotWidget->UpdateUI(nullptr, 0);
	}
	for (auto Item : ItemInventory)
	{
		UItemData* ItemData = Item.Key;
		FItemStatus Status = Item.Value;
		const auto SlotWidget = ItemSlots[Status.SlotID];
		SlotWidget->UpdateUI(ItemData, Status.Count);
	}
	if (ItemInventory.Num() > ItemSlots.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("more item data than slots"));
	}
}

void UInventoryWidget::Close()
{

	GetOwningPlayer()->SetShowMouseCursor(false);
	SetVisibility(ESlateVisibility::Hidden);
	bOpen = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());

}

void UInventoryWidget::ToggleOpenClose()
{
	UE_LOG(LogTemp, Log, TEXT("Toggle %d"), bOpen);
	if (bOpen)
		Close();
	else
		Open();
}
