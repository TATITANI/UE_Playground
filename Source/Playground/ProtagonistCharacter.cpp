// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProtagonistCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovementInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


//////////////////////////////////////////////////////////////////////////
AProtagonistCharacter::AProtagonistCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	///////////
	///// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MovementInfo = CreateDefaultSubobject<UMovementInfo>(TEXT("Movement"));
}

void AProtagonistCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//Add Input Mapping Context
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// clamp camera pitch	
		if (auto CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->ViewPitchMin = -45.0;
			CameraManager->ViewPitchMax = 45.0;
		}
	}

	
	MovementModeChangedDelegate.AddDynamic(this, &AProtagonistCharacter::OnChangedMovementMode);

	LandedDelegate.AddDynamic(this, &AProtagonistCharacter::OnLand);
}

//////////////////////////////////////////////////////////////////////////// Input

void AProtagonistCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::StopJumping);
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::Stop);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Look);
	
	}
}

void AProtagonistCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AProtagonistCharacter::Move(const FInputActionValue& Value)
{
	GetMovementComponent()->IsFalling();
	// input is a Vector2D
	MovementInfo->Dir = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementInfo->Dir.Y);
		AddMovementInput(RightDirection, MovementInfo->Dir.X);
	}
}

void AProtagonistCharacter::Stop(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Log, TEXT("STOP"));
	MovementInfo->Dir = {0, 0};
}

void AProtagonistCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AProtagonistCharacter::Jump()
{
	Super::Jump();
	MovementInfo->OnJump = true;
}

void AProtagonistCharacter::OnLand(const FHitResult& Hit)
{
	MovementInfo->OnJump = false;
}


void AProtagonistCharacter::OnChangedMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
                                                  uint8 PreviousCustomMode)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Error, TEXT("OnChangedMovementMode Character not valid"));
		return;
	}
	if (Character != this)
	{
		UE_LOG(LogTemp, Error, TEXT("OnChangedMovementMode character not match"));
		return;
	}

	MovementInfo->CurrentMovementMode = GetCharacterMovement()->MovementMode;

	UE_LOG(LogTemp, Log, TEXT("change moveMode : %s  -> %s"),
	       *UEnum::GetValueAsString(MovementInfo->CurrentMovementMode), *UEnum::GetValueAsString(PrevMovementMode));
}


void AProtagonistCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AProtagonistCharacter::GetHasRifle()
{
	return bHasRifle;
}
