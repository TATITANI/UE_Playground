// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYGROUND_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	typedef struct FTraceInfo
	{
		float Displacement;
		FRotator Rot;
		FVector HitPoint;
	};
	ACharacter *Character;
	float HalfHeightCapsule;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	FName LeftFootSocketName = "LeftFoot";

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	FName RightFootSocketName = "RightFoot";
	
	
public:	
	// Sets default values for this component's properties
	UFootIKComponent();

	FTraceInfo TraceFromFoot(FName SocketName);	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = "true"))
	float OffsetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = "true"))
	float OffsetRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = "true"))
	FRotator RotRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = "true"))
	FRotator RotLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = "true"))
	float HipDisplacement;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	

};
