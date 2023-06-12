// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProtagonistCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameInstance.h"
#include "Character/CharacterCurrentInfo.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/WeaponInventory.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MyHUD.h"
#include "UI/IngameWidget.h"


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

}

void AProtagonistCharacter::PostInitProperties()
{
	Super::PostInitProperties();
	CharacterCurrentInfo = NewObject<UCharacterCurrentInfo>();

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
	WeaponInventory = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->WeaponInventory;
	ensure(WeaponInventory != nullptr);

	AWeaponActor* DefaultWeaponActor = Cast<AWeaponActor>(GetWorld()->SpawnActor(DefaultWeapon));
	ensure(DefaultWeaponActor != nullptr);
	ObtainWeapon(DefaultWeaponActor);

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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::Stop);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProtagonistCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &AProtagonistCharacter::StopLookAround);

		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &AProtagonistCharacter::ClickChangeWeapon);
	}
}

void AProtagonistCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	auto BottomLoc = GetMesh()->GetSocketLocation("sd");
	
}

void AProtagonistCharacter::ObtainWeapon(AWeaponActor* WeaponActor)
{
	ensure(WeaponActor);
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	WeaponActor->AttachToComponent(GetMesh(), AttachmentRules, FName(WeaponActor->GetSocketName()));

	WeaponInventory->AddWeapon(WeaponActor);
	OnObtainWeapon.Broadcast(WeaponActor);

	ChangeWeapon(WeaponActor);
}

void AProtagonistCharacter::ClickChangeWeapon(const FInputActionValue& Value)
{
	const int8 SlotID = static_cast<int8>(Value.Get<float>()) - 1;;
	const auto MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	const auto WeaponType = MyHUD->IngameWidget->GetSlotWeaponType(SlotID);
	// UE_LOG(LogTemp,Log,TEXT("ClickChangeWeapon : %d, %d"), SlotID, WeaponType);
	if (WeaponType != WEAPON_None)
	{
		const auto WeaponActor = WeaponInventory->GetWeapon(WeaponType);
		ChangeWeapon(WeaponActor);
	}
}

void AProtagonistCharacter::ChangeWeapon(AWeaponActor* WeaponActor)
{
	if (WeaponInventory->HasWeapon(WeaponActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("%s weapon not in inventory"), *WeaponActor->GetName());
		return;
	}
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->UnUse();
	}

	CurrentWeapon = WeaponActor;
	WeaponActor->Use(this);
	CharacterCurrentInfo->SetCurrentWeaponType(WeaponActor->GetWeaponType());
	OnChangeWeapon.Broadcast(WeaponActor);
}

void AProtagonistCharacter::Move(const FInputActionValue& Value)
{
	CharacterCurrentInfo->Dir = FVector2D::Zero();

	if (!Movable)
		return;

	// input is a Vector2D
	CharacterCurrentInfo->Dir = Value.Get<FVector2D>();
	check(Controller != nullptr);
	if (Controller != nullptr)
	{
		// 좌우키 입력시 회전하면서 이동. 후진/카메라 회전하는 중에는 제외.
		if (CharacterCurrentInfo->Dir.X != 0 && CharacterCurrentInfo->Dir.Y != -1 && !IsLookingAround)
		{
			const auto SideAdjustedRot = GetControlRotation().Add(0, 0.5f * CharacterCurrentInfo->Dir.X, 0);
			Controller->SetControlRotation(SideAdjustedRot);
		}

		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * CharacterCurrentInfo->Dir.Y;
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * CharacterCurrentInfo->Dir.X;
		FVector Direction = ForwardDirection + RightDirection;
		Direction.Normalize();

		AddMovementInput(Direction);
	}
}

void AProtagonistCharacter::Stop(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Log, TEXT("STOP"));
	CharacterCurrentInfo->Dir = {0, 0};
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

		const auto ActorRotation = GetActorRotation();
	}
}

void AProtagonistCharacter::StopLookAround(const FInputActionValue& Value)
{
	IsLookingAround = false;
}


void AProtagonistCharacter::Jump()
{
	Super::Jump();
	CharacterCurrentInfo->OnJump = true;
}

void AProtagonistCharacter::OnLand(const FHitResult& Hit)
{
	CharacterCurrentInfo->OnJump = false;
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

	CharacterCurrentInfo->CurrentMovementMode = GetCharacterMovement()->MovementMode;
	UE_LOG(LogTemp, Log, TEXT("change moveMode : %s  -> %s"),
	       *UEnum::GetValueAsString(PrevMovementMode), *UEnum::GetValueAsString(CharacterCurrentInfo->CurrentMovementMode));
}
