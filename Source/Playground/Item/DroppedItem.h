// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroppedItem.generated.h"

UCLASS()
class PLAYGROUND_API ADroppedItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADroppedItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Init(FVector Loc);

	void Activate();

private:
	class UPlaygroundItem* Item;

	UPROPERTY(VisibleAnywhere)
	int32 ItemCnt = 1;

	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent;
};
