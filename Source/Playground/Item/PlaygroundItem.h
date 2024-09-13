// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "PlaygroundItem.generated.h"

UCLASS()
class PLAYGROUND_API APlaygroundItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaygroundItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Init(FVector Loc, FItemStatus _ItemStatus);

private:
	UPROPERTY(EditAnywhere, Category=Item)
	UItemData* ItemData;

	UPROPERTY(VisibleAnywhere)
	FItemStatus ItemStatus;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComponent;

public:
	UPROPERTY(VisibleDefaultsOnly, Category= Sound, meta=(AllowPrivateAccess))
	USoundBase* ObtainSound;
	
protected:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                          bool bFromSweep, const FHitResult& SweepResult);
};
