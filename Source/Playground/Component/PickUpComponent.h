// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "PickUpComponent.generated.h"

class AWeaponActor;
// Declaration of the delegate that will be called when someone picks this up
// The character picking this up is the parameter sent with the notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AProtagonistCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UPickUpComponent : public USphereComponent
{
	GENERATED_BODY()

	
private:

public:
	UPROPERTY()
	AWeaponActor* WeaponActor;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FOnPickUp OnPickUp;

	UPickUpComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

	/** Code for when something overlaps this component */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                          bool bFromSweep, const FHitResult& SweepResult);
};
