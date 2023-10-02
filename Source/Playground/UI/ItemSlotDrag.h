// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemSlotDrag.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UItemSlotDrag : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// class UItemData* ItemData;
	int32 SlotID;
	
};
