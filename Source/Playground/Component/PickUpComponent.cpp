// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickUpComponent.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Component/CharacterWeaponComponent.h"


UPickUpComponent::UPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UPickUpComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Register our Overlap Event
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPickUpComponent::OnSphereBeginOverlap);
	WeaponActor = Cast<AWeaponActor>(GetOwner());
	verify(WeaponActor != nullptr);

}

void UPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProtagonistCharacter* ProtagonistCharacter = Cast<AProtagonistCharacter>(OtherActor);
	if (ProtagonistCharacter != nullptr)
	{
		ProtagonistCharacter->Weapon->ObtainWeapon(WeaponActor);
		
		// Notify that the actor is being pickekd up
		OnPickUp.Broadcast(ProtagonistCharacter);
		
		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
