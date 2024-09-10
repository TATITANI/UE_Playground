// Fill out your copyright notice in the Description page of Project Settings.


#include "Boids/BoidEntity.h"
#include "BoidsGenerator.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

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
}

void ABoidEntity::Init(ABoidEntity* _Leader, FVector _Pivot, float _MovableRadius, float _Speed, const FBoidsWeight _BoidsWeight)
{
	Leader = _Leader;
	Pivot = _Pivot;
	MovableRadius = _MovableRadius;
	BoidsWeight = _BoidsWeight;
	Speed = _Speed;
}


void ABoidEntity::CalculateVelocity()
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
		NeighborsCnt != 0 ? SumNeighborsVelocity / NeighborsCnt : CurrentVelocity;
	const FVector CenterNeighbors = NeighborsCnt != 0 ? SumNeighborsLoc / NeighborsCnt : GetActorLocation();

	const FVector CohesionVector = (CenterNeighbors - GetActorLocation()).GetSafeNormal();
	const FVector AlignmentVector = (AverageNeighborsVelocity - CurrentVelocity).GetSafeNormal();
	const FVector SeperationVector = -(SumNeighborsLoc - GetActorLocation() * NeighborsCnt).GetSafeNormal();
	const FVector LeaderFollowingVector = (Leader->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	RandomVector = FMath::VRand();

	TargetVelocity = Speed * (CohesionVector * BoidsWeight.WeightCohesion
		+ AlignmentVector * BoidsWeight.WeightAlignment
		+ SeperationVector * BoidsWeight.WeightSeperation
		+ RandomVector * BoidsWeight.WeightRandomMove
		+ LeaderFollowingVector * BoidsWeight.WeightLeaderFollowing);
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

	if (FHitResult HitResult; CheckObstacle(HitResult))
	{
		CurrentVelocity = TargetVelocity = FMath::GetReflectionVector(CurrentVelocity, HitResult.Normal);
	}

	// if ( FVector::Distance(GetActorLocation(), Pivot) > MovableRadius)
	// {
	// 	CurrentVelocity = TargetVelocity = FVector((Pivot - GetActorLocation()).GetSafeNormal()) * Speed * 2;
	// }

	CurrentVelocity = FMath::VInterpTo(CurrentVelocity, TargetVelocity, DeltaTime, 5);
	AddActorWorldOffset(CurrentVelocity * DeltaTime);

	const auto Rot = FRotationMatrix::MakeFromX(CurrentVelocity).Rotator();
	SetActorRotation(Rot);
}
