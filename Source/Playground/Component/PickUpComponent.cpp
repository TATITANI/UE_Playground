// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickUpComponent.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"

UPickUpComponent::UPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UPickUpComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnSphereBeginOverlap);
	WeaponActor = Cast<AWeaponActor>(GetOwner());
	verify(WeaponActor != nullptr);

}

void UPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProtagonistCharacter* Character = Cast<AProtagonistCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->ObtainWeapon(WeaponActor);
		
		// Notify that the actor is being pickekd up
		OnPickUp.Broadcast(Character);
		
		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
