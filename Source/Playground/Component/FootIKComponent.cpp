// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FootIKComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UFootIKComponent::UFootIKComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
	HalfHeightCapsule = Cast<UCapsuleComponent>(Character->GetComponentByClass(UCapsuleComponent::StaticClass()))->GetScaledCapsuleHalfHeight();
}


// Called every frame
void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto LeftTraceResult = TraceFromFoot(LeftFootSocketName);
	const auto RightTraceResult = TraceFromFoot(RightFootSocketName);

	const float CurrentHipDisplacement = FMath::Min(LeftTraceResult.Displacement, RightTraceResult.Displacement);
	HipDisplacement = FMath::FInterpTo(HipDisplacement, CurrentHipDisplacement, DeltaTime, 10);
	
	//test
	OffsetLeft = (-LeftTraceResult.Displacement + HipDisplacement) * -1;
	OffsetRight = (-RightTraceResult.Displacement + HipDisplacement) * -1;
	// UE_LOG(LogTemp,Log,TEXT("offset : %f, %f"), OffsetLeft, OffsetRight);

	RotLeft = FMath::RInterpTo(RotLeft, LeftTraceResult.Rot, DeltaTime, 10);
	RotRight = FMath::RInterpTo(RotRight, FRotator(-RightTraceResult.Rot.Pitch, RightTraceResult.Rot.Yaw, RightTraceResult.Rot.Roll), DeltaTime, 10);
}


UFootIKComponent::FTraceInfo UFootIKComponent::TraceFromFoot(FName SocketName)
{
	UFootIKComponent::FTraceInfo TraceInfo;

	//! Set Linetraces startpoint and end point
	FVector SocketLocation = Character->GetMesh()->GetSocketLocation(SocketName);
	FVector Line_Start = FVector(SocketLocation.X, SocketLocation.Y, (Character->GetActorLocation().Z));
	FVector Line_End = Line_Start - FVector(0, 0, HalfHeightCapsule*2 );

	//! Process Line Trace
	FHitResult HitResult;

	bool IsHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Line_Start, Line_End,
	                                                   UEngineTypes::ConvertToTraceType(ECC_Visibility), true,
	                                                   {GetOwner()}, EDrawDebugTrace::None /*ForOneFrame*/, HitResult, true);

	//! Set ImpactNormal and Offset from HitResult
	if (HitResult.IsValidBlockingHit() == true)
	{
		TraceInfo.HitPoint = HitResult.ImpactPoint;
		float HitLength = (HitResult.ImpactPoint - HitResult.TraceEnd).Size();

		TraceInfo.Displacement = HitLength - HalfHeightCapsule;
	}
	else
	{
		TraceInfo.Displacement = 0.0f;
	}

	auto DegXZ = UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);
	auto DegYZ = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
	TraceInfo.Rot = FRotator(-DegXZ, 0, DegYZ);
	
	return TraceInfo;
}
