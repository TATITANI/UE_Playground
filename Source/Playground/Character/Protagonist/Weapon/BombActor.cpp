// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/BombActor.h"

#include "FlyingBomb.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "InputTriggers.h"
#include "MyGameInstance.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"


ABombActor::ABombActor()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SplineComponent->SetupAttachment(RootComponent);
}

void ABombActor::BeginPlay()
{
	Super::BeginPlay();
	InitSplineMeshes();

	// stat
	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
	auto BombStat = GameInstance->GetWeaponStat<FBombStat>(GetWeaponType(), FName("1"));
	this->Speed = BombStat->Speed;
	
}


void ABombActor::AttackInputCompleted()
{
	Super::AttackInputCompleted();
	Throw();

}

void ABombActor::InitSplineMeshes()
{
	const int32 PointCnt = FMath::TruncToInt(TrajectoryParams.SimFrequency * TrajectoryParams.MaxSimTime);

	for (int32 i = 0; i < PointCnt - 1; i++)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>();
		NewSplineMesh->SetStaticMesh(SplineStaticMesh);
		NewSplineMesh->SetMobility(EComponentMobility::Movable);
		// NewSplineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewSplineMesh->RegisterComponentWithWorld(GetWorld()); // 런타임 적용
		NewSplineMesh->SetStartScale(FVector2d(0.1, 0.1));
		NewSplineMesh->SetEndScale(FVector2d(0.1, 0.1));
		NewSplineMesh->SetSimulatePhysics(false);
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewSplineMesh->SetVisibility(false);
		NewSplineMesh->SetMaterial(0, TrajectoryMaterialInstance);
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::Z);
		SplineMeshes.Add(NewSplineMesh);
	}
}

FVector ABombActor::GetThrowingStartLocation()
{
	return GetActorLocation();
}



void ABombActor::UnEquip()
{
	Super::UnEquip();
	SetVisibleTrajectory(false);

	SetAimMovement(false);
}

void ABombActor::SetVisibleTrajectory(bool bVisible)
{
	for (const auto& Mesh : SplineMeshes)
	{
		Mesh->SetVisibility(bVisible);
	}
	
}


FVector ABombActor::GetThrowingVelocity() const
{
	// 컨트롤러가 앞을 바라볼 때 방향벡터
	const FVector Dir = FRotationMatrix(Protagonist->GetControlRotation()).GetScaledAxis(EAxis::X);

	// UE_LOG(LogTemp,Log, TEXT("Throw Dir : %s"), *Dir.ToString());
	const auto Velocity = Dir * Speed;
	return Velocity;
}


void ABombActor::AttackInputStarted()
{
	SetAimMovement(true);
	
	
}

void ABombActor::AttackInputTrigger()
{
	Super::AttackInputTrigger();

	FPredictProjectilePathResult PathResult;
	TrajectoryParams.LaunchVelocity = GetThrowingVelocity();
	TrajectoryParams.StartLocation = GetThrowingStartLocation();
	TrajectoryParams.ProjectileRadius = FlyingBombClass->GetDefaultObject<AFlyingBomb>()->GetRadius();

	bool IsHit = UGameplayStatics::PredictProjectilePath(this, TrajectoryParams, PathResult);
	SplineComponent->ClearSplinePoints();

	for (int32 i = 0; i < PathResult.PathData.Num(); i++)
	{
		SplineComponent->AddSplinePoint(PathResult.PathData[i].Location, ESplineCoordinateSpace::World);
		SplineComponent->SetSplinePointType(i, ESplinePointType::Curve);
	}

	for (int32 i = 0; i < SplineMeshes.Num(); i++)
	{
		auto StartLoc = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		auto StartTan = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		auto EndLoc = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
		auto EndTan = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
		SplineMeshes[i]->SetStartAndEnd(StartLoc, StartTan, EndLoc, EndTan);
		SplineMeshes[i]->SetVisibility(true);
	}
}

void ABombActor::SetAimMovement(bool IsAim) const
{
	Protagonist->SetMovable(!IsAim);
	Protagonist->SetUsingControllerYaw(IsAim);
}


void ABombActor::Throw()
{
	SetAimMovement(false);
	
	ensure(ThrowMontage != nullptr);
	ensure(AnimInstance != nullptr);
	if (ThrowMontage != nullptr && AnimInstance != nullptr)
	{
		AnimInstance->Montage_Play(ThrowMontage, 1.f);
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	const auto FlyingBomb = GetWorld()->SpawnActor<AFlyingBomb>(FlyingBombClass, GetThrowingStartLocation(), FRotator::ZeroRotator, ActorSpawnParams);
	if (FlyingBomb != nullptr)
	{
		FlyingBomb->Fly(GetThrowingVelocity(), Damage);
	}
	SetVisibleTrajectory(false);
}
