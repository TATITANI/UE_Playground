// Fill out your copyright notice in the Description page of Project Settings.


#include "Boids/BoidEntity.h"

// Sets default values
ABoidEntity::ABoidEntity()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABoidEntity::BeginPlay()
{
	Super::BeginPlay();
	Dir = GetActorForwardVector();
}

void ABoidEntity::Init(ABoidEntity* _Leader, FVector _Pivot, float _MovableRadius)
{
	Leader = _Leader;
	Pivot = _Pivot;
	MovableRadius = _MovableRadius;

	GetWorldTimerManager().SetTimer(RandomMovementTimerHandle, this, &ABoidEntity::UpdateRandomMovement, 1, true, 0.5f);
}

void ABoidEntity::UpdateRandomMovement()
{
	RandomVector = FMath::VRand();
}

void ABoidEntity::CalculateDir()
{
	TArray<FOverlapResult> OverlapResults;
	const FCollisionObjectQueryParams ObjectParams(TraceCollisionChannel);
	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(500.0f));
	// DrawDebugSphere(GetWorld(), GetActorLocation(), 1500, 16, FColor::Magenta);
	FVector SumNeighborsLoc = FVector::ZeroVector;
	FVector SumNeighborsVelocity = FVector::ZeroVector;

	int NeighborsCnt = 0;
	for (auto& OverlapResult : OverlapResults)
	{
		const AActor* OverlappedActor = OverlapResult.GetActor();
		if (bool IsBoidEntity = OverlappedActor->IsA(StaticClass()))
		{
			SumNeighborsLoc += OverlappedActor->GetActorLocation();
			SumNeighborsVelocity += Cast<ABoidEntity>(OverlappedActor)->GetVelocity();
			NeighborsCnt++;
		}
	}

	const FVector AverageNeighborsVelocity =
		NeighborsCnt != 0 ? SumNeighborsVelocity / NeighborsCnt : Velocity;
	const FVector CenterNeighbors = NeighborsCnt != 0 ? SumNeighborsLoc / NeighborsCnt : GetActorLocation();

	const FVector CohesionVector = (CenterNeighbors - GetActorLocation()).GetSafeNormal();
	const FVector AlignmentVector = (AverageNeighborsVelocity - Velocity).GetSafeNormal();
	const FVector SeperationVector = -(SumNeighborsLoc - GetActorLocation() * NeighborsCnt).GetSafeNormal();
	const FVector LeaderFollowingVector = (Leader->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	Dir = (CohesionVector * WeightCohesion + AlignmentVector * WeightAlignment + SeperationVector * WeightSeperation
		+ RandomVector * WeightRandomMove + LeaderFollowingVector * WeightLeaderFollowing).GetSafeNormal();
}

bool ABoidEntity::CheckObstacle(FHitResult& HitResult)
{
	FVector LineStartLoc = GetActorLocation(), LineEndLoc = LineStartLoc + GetActorForwardVector() * 1000;
	GetWorld()->LineTraceSingleByChannel(HitResult, LineStartLoc, LineEndLoc, ECC_WorldStatic);
	FColor LineColor = HitResult.bBlockingHit ? FColor::Green : FColor::Red;
	// DrawDebugLine(GetWorld(), LineStartLoc, LineEndLoc, LineColor);


	return HitResult.bBlockingHit;
}


// Called every frame
void ABoidEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// FVector Dir = CalculateDir();
	
	if (FHitResult HitResult; CheckObstacle(HitResult))
	{
		Dir = RandomVector = FMath::GetReflectionVector(GetActorForwardVector(), HitResult.Normal);
	}
	if (FVector::Distance(GetActorLocation(), Pivot) > MovableRadius)
	{
		Dir = RandomVector = (Pivot - GetActorLocation()).GetSafeNormal();
	}

	const FVector CurrentDir = FMath::VInterpTo(GetActorForwardVector(), Dir, DeltaTime, 5);
	Velocity = CurrentDir * Speed * DeltaTime;
	AddActorWorldOffset(Velocity);


	const auto Rot = FRotationMatrix::MakeFromX(CurrentDir).Rotator();
	SetActorRotation(Rot);
}
