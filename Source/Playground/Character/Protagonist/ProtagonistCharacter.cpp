// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProtagonistCharacter.h"

#include "Component/ClimbComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameInstance.h"

#include "Character/CharacterCurrentInfo.h"
#include "Component/CharacterWeaponComponent.h"
#include "Component/DashComponent.h"
#include "Component/FootIKComponent.h"
#include "Component/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/UtilPlayground.h"

//////////////////////////////////////////////////////////////////////////
AProtagonistCharacter::AProtagonistCharacter()
{
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

	WeaponComponent = CreateDefaultSubobject<UCharacterWeaponComponent>(TEXT("Weapon"));
	Climbing = CreateDefaultSubobject<UClimbComponent>(TEXT("Climb"));
	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("Dash"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	FootIKComponent = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIK"));
}

void AProtagonistCharacter::PostInitProperties()
{
	Super::PostInitProperties();
	// CharacterCurrentInfo = NewObject<FCharacterCurrentInfo>();
}

void AProtagonistCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AProtagonistCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//Add Input Mapping Context
	if (PlayerController)
	{
		Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		ensure(Subsystem!=nullptr);
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, InputPriority);
		}

		// clamp camera pitch	
		if (auto CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->ViewPitchMin = -70.0;
			CameraManager->ViewPitchMax = 70.0;
		}
	}

	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
	auto ProtaonistStat = GameInstance->GetCharacterStat<FProtagonistStat>(ECharacterStatType::Protagonist, "1");
	HealthComponent->Init(ProtaonistStat->MaxHp);
	HealthComponent->OnHpChanged.AddLambda([this](int32 CurrentHp, int32 DelthHp, int32 MaxHp)
	{
		if (DelthHp < 0)
			CharacterCurrentInfo.OnHit = true;
	});


	MovementModeChangedDelegate.AddUniqueDynamic(this, &AProtagonistCharacter::OnChangedMovementMode);
	LandedDelegate.AddUniqueDynamic(this, &AProtagonistCharacter::OnLand);

	
}


//////////////////////////////////////////////////////////////////////////// Input

void AProtagonistCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AProtagonistCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::StopJumping);
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::GroundMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::Stop);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::StopLookAround);
	}
}

void AProtagonistCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AProtagonistCharacter::GroundMove(const FInputActionValue& Value)
{
	CharacterCurrentInfo.InputDir = FVector2D::Zero();

	if (!Movable)
		return;

	// input is a Vector2D
	CharacterCurrentInfo.InputDir = Value.Get<FVector2D>();
	check(Controller != nullptr);
	if (Controller != nullptr)
	{
		// 좌우키 입력시 회전하면서 이동. 후진/카메라 회전하는 중에는 제외.
		if (CharacterCurrentInfo.InputDir.X != 0 && CharacterCurrentInfo.InputDir.Y != -1 && !IsLookingAround)
		{
			const auto SideAdjustedRot = GetControlRotation().Add(0, 0.5f * CharacterCurrentInfo.InputDir.X, 0);
			Controller->SetControlRotation(SideAdjustedRot);
		}

		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * CharacterCurrentInfo.InputDir.Y;
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * CharacterCurrentInfo.InputDir.X;
		FVector Direction = ForwardDirection + RightDirection;
		Direction.Normalize();

		AddMovementInput(Direction);
	}
}


void AProtagonistCharacter::Stop(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Log, TEXT("STOP"));
	CharacterCurrentInfo.InputDir = {0, 0};
	GetCharacterMovement()->StopMovementImmediately();
}

void AProtagonistCharacter::Look(const FInputActionValue& Value)
{
	IsLookingAround = true;
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProtagonistCharacter::StopLookAround(const FInputActionValue& Value)
{
	IsLookingAround = false;
}


void AProtagonistCharacter::Jump()
{
	Super::Jump();
	CharacterCurrentInfo.OnBeginJump = true;
}

void AProtagonistCharacter::ZoomOnSlash_Implementation()
{
}

void AProtagonistCharacter::AimCamByWeapon_Implementation(EWeaponType WeaponType)
{
}


void AProtagonistCharacter::OnLand(const FHitResult& Hit)
{
}


void AProtagonistCharacter::OnChangedMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
                                                  uint8 PreviousCustomMode)
{
	ensureMsgf(IsValid(Character), TEXT("OnChangedMovementMode character not valid"));
	if (!IsValid(Character))
		return;

	ensureMsgf(Character == this, TEXT("OnChangedMovementMode character not match"));
	if (Character != this)
		return;

	CharacterCurrentInfo.CurrentMovementMode = GetCharacterMovement()->MovementMode;
	// UE_LOG(LogTemp, Log, TEXT("change moveMode : %s  -> %s"),
	//        *UEnum::GetValueAsString(PrevMovementMode), *UEnum::GetValueAsString(CharacterCurrentInfo.CurrentMovementMode));
}
