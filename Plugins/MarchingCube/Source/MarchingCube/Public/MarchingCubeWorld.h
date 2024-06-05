// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewMesh.h"
#include "Generators/MarchingCubes.h"
#include "DynamicMesh/Public/DynamicMeshEditor.h"
#include "Math/GenericOctree.h"

#include "MarchingCubeWorld.generated.h"

using MapOctreeID = TMap<int32, FOctreeElementId2>; // VertexID, OctreeID

USTRUCT(BlueprintType)
struct FMarchingCubeProperty
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UMaterialInstance* Material;

	UPROPERTY(EditAnywhere)
	FVector Location;

	UPROPERTY(EditAnywhere, meta=( ClampMin= 0.001f))
	float NoiseScale = 0.01f;

	UPROPERTY(EditAnywhere, meta=( ClampMin= 0.001f), Category="Terrain")
	float Height = 100;

	UPROPERTY(EditAnywhere, meta=( ClampMin= 0.001f, ClampMax = 1), Category="Cube")
	float Threshold = 0.2f;

	UPROPERTY(EditAnywhere)
	double CubeSize = 10;

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


// octree
struct FMarchingCubeMeshVertex
{
	FMarchingCubeMeshVertex(int32 InVertexID, FVector InPosition, MapOctreeID* InMapID)
	{
		VertexID = InVertexID;
		Location = InPosition;
		MapID = InMapID;
	}

	FVector Location;
	int32 VertexID;

	MapOctreeID* MapID;
};

/** Helper struct for the mesh component vert position octree */
struct FMarchingCubeOctreeSemantics
{
	enum { MaxElementsPerLeaf = 16 };

	enum { MinInclusiveElementsPerNode = 7 };

	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	/**
	 * Get the bounding box of the provided octree element. In this case, the box
	 * is merely the point specified by the element.
	 *
	 * @param	Element	Octree element to get the bounding box for
	 *
	 * @return	Bounding box of the provided octree element
	 */
	FORCEINLINE static FBoxCenterAndExtent GetBoundingBox(const FMarchingCubeMeshVertex& Element)
	{
		return FBoxCenterAndExtent(Element.Location, FVector::ZeroVector);
	}

	/** Ignored for this implementation */
	FORCEINLINE static void SetElementId(const FMarchingCubeMeshVertex& Element, FOctreeElementId2 Id)
	{
		Element.MapID->Add(Element.VertexID, Id);
		
		// UE_LOG(LogTemp,Log,TEXT("Set nodeID : %d"), Id.GetNodeIndex());
	}
};

//

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

	void GenerateCubeWorld(FMarchingCubeProperty& MarchingCubeProperty, const FVector3d& InBoundSize);

	void GenerateTerrainWorld(FMarchingCubeProperty& MarchingCubeProperty, const FVector3d& InBoundSize);


	void Sculpt(FVector BrushLocation, FMarchingCubeSculptProperty& SculptProperty);
	void Erode(FVector BrushLocation, FMarchingCubeErodeProperty& ErodeProperty, FVector3d ErodeDir);
	void ErodeLegacy(FVector BrushLocation, FMarchingCubeErodeProperty& ErodeProperty, FVector3d ErodeDir);

private:
	void Init(FMarchingCubeProperty& MarchingCubeProperty, const FVector3d& InBoundSize,
	          TFunction<double(UE::Math::TVector<double>)> Implicit);

	void GenerateMarchingCubeData(UE::Geometry::FMarchingCubes& MarchingCubes, TFunction<double(UE::Math::TVector<double>)> Implicit,
	                              FIntVector& SeedGap, double CubeSize = 10);

	FDynamicMesh3* CreateDynamicMesh(UE::Geometry::FMarchingCubes& MarchingCubes);
	void CreateProceduralMesh(UE::Geometry::FMarchingCubes& MarchingCubes, UMaterial* Material, int32 SectionID = 0);

	TFunction<double(UE::Math::TVector<double>)> ImplicitTerrain(float NoiseScale, float Height);
	TFunction<double(UE::Math::TVector<double>)> ImplicitSphere(float Radius, FVector BrushLocation = FVector::ZeroVector);
	TFunction<double(UE::Math::TVector<double>)> ImplicitCube(float NoiseScale, float Threshold);

private:
	UE::Geometry::FMeshIndexMappings IndexMappings;
	FVector3d BoundSize;

	using MarchingCubeMeshOctree = TOctree2<FMarchingCubeMeshVertex, FMarchingCubeOctreeSemantics>;
	MarchingCubeMeshOctree MeshOctree;
	MapOctreeID OctreeIdMap;
	
	void InitOctree();
};
