// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/CharacterCurrentInfo.h"
#include "ProtagonistCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


UCLASS(config=Game)
class AProtagonistCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProtagonistCharacter();

private:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnChangedMovementMode(class ACharacter* Character, EMovementMode PrevMovementMode,
	                           uint8 PreviousCustomMode);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UFUNCTION()
	void OnLand(const FHitResult& Hit);
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Stop(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StopLookAround(const FInputActionValue& Value);
	virtual void Jump() override;

private:
	//** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	bool IsLookingAround = false;
	bool Movable = true;

	UPROPERTY(EditAnywhere, Category=Weapon, meta=(AllowPrivateAccess=true))
	TSubclassOf<class AWeaponActor> DefaultWeapon;
	

	UPROPERTY(VisibleAnywhere, Category=Weapon, meta=(AllowPrivateAccess = true))
	TArray<class AWeaponActor*> WeaponInventory;

public:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCharacterCurrentInfo* CharacterCurrentInfo;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	virtual void Tick(float DeltaSeconds) override;

	void SetMovable(bool bEnable){this->Movable = bEnable;}
};
