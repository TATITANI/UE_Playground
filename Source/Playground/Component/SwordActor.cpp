// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SwordActor.h"

#include "EnhancedInputComponent.h"

void ASwordActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &ASwordActor::Attack);
}

void ASwordActor::Attack()
{
	if (AttackMontage != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(AttackMontage, 1.f);
		}
	}
}
