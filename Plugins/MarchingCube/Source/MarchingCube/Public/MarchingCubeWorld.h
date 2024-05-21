// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewMesh.h"
#include "Generators/MarchingCubes.h"
#include "DynamicMesh/Public/DynamicMeshEditor.h"

#include "MarchingCubeWorld.generated.h"


USTRUCT(BlueprintType)
struct FMarchingCubeProperty
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UMaterialInstance* Material;

	UPROPERTY(EditAnywhere, meta=( ClampMin= 0.001f))
	float NoiseScale = 0.01f;

	UPROPERTY(EditAnywhere)
	float Height = 100;

	UPROPERTY(EditAnywhere)
	FVector Location;

	FIntVector& GetSeedGap();

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	FIntVector SeedGap = FIntVector(20, 20, 20);
};

USTRUCT(BlueprintType)
struct FMarchingCubeSculptProperty
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UMaterialInstance* Material;

	UPROPERTY(EditAnywhere)
	float BrushRadius = 30;

	UPROPERTY(EditAnywhere)
	FIntVector SeedGap = FIntVector(20, 20, 20);
};


USTRUCT(BlueprintType)
struct FMarchingCubeErodeProperty
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float BrushRadius = 10;

	UPROPERTY(EditAnywhere)
	double Strength = 2;
};


UCLASS()
class MARCHINGCUBE_API AMarchingCubeWorld : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMarchingCubeWorld();
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class UDynamicMeshComponent* DynamicMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="New MarchingCube")
	FVector Position;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitProperties() override;
	virtual void PostInitializeComponents() override;

	void Init(FMarchingCubeProperty& MarchingCubeProperty, const FVector3d& InBoundSize);

	void Sculpt(FVector Location, FMarchingCubeSculptProperty& SculptProperty);
	void Erode(FVector BrushLocation, FMarchingCubeErodeProperty& ErodeProperty, FVector3d ErodeDir);

private:
	void GenerateMarchingCubeData(UE::Geometry::FMarchingCubes& MarchingCubes, TFunction<double(UE::Math::TVector<double>)> Implicit,
	                              FIntVector& SeedGap, double CubeSize = 10);

	FDynamicMesh3* CreateDynamicMesh(UE::Geometry::FMarchingCubes& MarchingCubes);
	void CreateProceduralMesh(UE::Geometry::FMarchingCubes& MarchingCubes, UMaterial* Material, int32 SectionID = 0);

	TFunction<double(UE::Math::TVector<double>)> ImplicitTerrain(float NoiseScale, float Height);
	TFunction<double(UE::Math::TVector<double>)> ImplicitSphere(float Radius, FVector BrushLocation = FVector::ZeroVector);

private:
	UE::Geometry::FMeshIndexMappings IndexMappings;
	FVector3d BoundSize;
};
