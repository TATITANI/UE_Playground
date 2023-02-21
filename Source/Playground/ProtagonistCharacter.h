// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ProtagonistCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


UCLASS(BlueprintType)
class UMovementInfo : public UObject
{
	GENERATED_BODY()

private:
	friend class AProtagonistCharacter;

	UPROPERTY(BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	FVector2D Dir = FVector2d::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	bool OnJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	TEnumAsByte<enum EMovementMode> CurrentMovementMode;

public:
	FORCEINLINE FVector2D GetDir() const { return Dir; }
};

UCLASS(config=Game)
class AProtagonistCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProtagonistCharacter();

private:
	virtual void BeginPlay();

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

private:
	UFUNCTION()
	void OnChangedMovementMode(class ACharacter* Character, EMovementMode PrevMovementMode,
	                           uint8 PreviousCustomMode);

	UFUNCTION()
	void OnLand(const FHitResult& Hit);

public:
	int32 AmmoCount = 5;
	UMovementInfo* MovementInfo;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

private:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Stop(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Jump() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	virtual void Tick(float DeltaSeconds) override;
};
