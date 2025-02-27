// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/ProtagonistMovementComponent.h"

#include "ProtagonistCharacter.h"
#include "Net/UnrealNetwork.h"


void FSavedMove_Protagonist::Clear()
{
	Super::Clear();
	bFixLocation = false;
}


bool FSavedMove_Protagonist::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_Protagonist* ProtagonistMove = static_cast<const FSavedMove_Protagonist*>(NewMove.Get());

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_Protagonist::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
                                        FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	AProtagonistCharacter* Protagonist = Cast<AProtagonistCharacter>(Character);
	if (Protagonist)
	{
	}
}

// bUpdatePosition이 true일때
void FSavedMove_Protagonist::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UE_LOG(LogPGNetwork, Warning, TEXT(""));
	AProtagonistCharacter* Protagonist = Cast<AProtagonistCharacter>(Character);
	if (Protagonist)
	{
	}
}


FNetworkPredictionData_Client_Protagonist::FNetworkPredictionData_Client_Protagonist(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_Protagonist::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Protagonist());
}


/*** UProtagonistMovementComponent ***/

UProtagonistMovementComponent::UProtagonistMovementComponent()
{
	SetIsReplicatedByDefault(true);
}


FNetworkPredictionData_Client* UProtagonistMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UProtagonistMovementComponent* MutableThis = const_cast<UProtagonistMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Protagonist(*this);
	}
	return ClientPredictionData;
}

void UProtagonistMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (bFixLocation)
	{
		Velocity = FVector::Zero();
	}
	GravityScale = bFixLocation ? 0 : 4;
}

void UProtagonistMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UProtagonistMovementComponent, bFixLocation);
}

void UProtagonistMovementComponent::OnRep_FixLocation()
{
}

void UProtagonistMovementComponent::ServerFixLocation_Implementation(bool InFixLocation)
{
	bFixLocation = InFixLocation;
}
