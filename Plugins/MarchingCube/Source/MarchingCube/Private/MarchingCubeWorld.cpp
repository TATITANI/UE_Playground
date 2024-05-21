// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCubeWorld.h"

#include "Generators/SphereGenerator.h"
#include "GeometryFramework/Public/Components/DynamicMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "ModelingComponentsEditorOnly/Public/EditorModelingObjectsCreationAPI.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "ProceduralMeshComponent/Public/KismetProceduralMeshLibrary.h"
#include "GeometryCore/Public/IndexTypes.h"

FIntVector& FMarchingCubeProperty::GetSeedGap()
{
	if (SeedGap.X <= 0 || SeedGap.Y <= 0 || SeedGap.Z <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SeedGap must be great than zero"));
		SeedGap = FIntVector(10, 10, 10);
	}

	return SeedGap;
}

// Sets default values
AMarchingCubeWorld::AMarchingCubeWorld()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("Dynamic"));
	DynamicMeshComponent->SetupAttachment(RootComponent);

	DynamicMeshComponent->bUseAsyncCooking = true;
	DynamicMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DynamicMeshComponent->EnableComplexAsSimpleCollision();
	DynamicMeshComponent->SetGenerateOverlapEvents(true);
	DynamicMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	DynamicMeshComponent->bEnableComplexCollision = true;


#pragma region ProceduralMeshComponent
	// ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	// ProceduralMeshComponent->SetupAttachment(GetRootComponent());
	// ProceduralMeshComponent->bUseAsyncCooking = true;
	// ProceduralMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// ProceduralMeshComponent->SetGenerateOverlapEvents(true);
	// ProceduralMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);F#
#pragma endregion
}

void AMarchingCubeWorld::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


// Called when the game starts or when spawned
void AMarchingCubeWorld::BeginPlay()
{
	Super::BeginPlay();
	DynamicMeshComponent->UpdateCollision(false);
}

// Called every frame
void AMarchingCubeWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMarchingCubeWorld::PostInitProperties()
{
	Super::PostInitProperties();
}

void AMarchingCubeWorld::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMarchingCubeWorld::Init(FMarchingCubeProperty& MarchingCubeProperty, const FVector3d& InBoundSize)
{
	UE::Geometry::FMarchingCubes MarchingCubes;

	BoundSize = InBoundSize;

	const TFunction<double(UE::Math::TVector<double>)> Implicit = ImplicitTerrain(MarchingCubeProperty.NoiseScale, MarchingCubeProperty.Height);
	GenerateMarchingCubeData(MarchingCubes, Implicit, MarchingCubeProperty.GetSeedGap());

	// CreateProceduralMesh(MarchingCubes);

	FDynamicMesh3* Mesh = CreateDynamicMesh(MarchingCubes);
	DynamicMeshComponent->SetMesh(MoveTemp(*Mesh));

	DynamicMeshComponent->FastNotifyUVsUpdated();
	DynamicMeshComponent->NotifyMeshUpdated();
	DynamicMeshComponent->SetMaterial(0, MarchingCubeProperty.Material);

	SetActorLocation(MarchingCubeProperty.Location);
}

void AMarchingCubeWorld::GenerateMarchingCubeData(UE::Geometry::FMarchingCubes& MarchingCubes, TFunction<double(UE::Math::TVector<double>)> Implicit,
                                                  FIntVector& SeedGap, double CubeSize)
{
	// 계산 편의상 z는 0부터 시작
	const FVector3d MinBound = BoundSize * -0.5f;
	const FVector3d MaxBound = BoundSize * 0.5f;

	MarchingCubes.Reset();
	MarchingCubes.IsoValue = 0; // 등가면 레벨 설정
	MarchingCubes.Bounds = UE::Geometry::TAxisAlignedBox3(MinBound, MaxBound);
	MarchingCubes.RootMode = UE::Geometry::ERootfindingModes::SingleLerp;
	MarchingCubes.RootModeSteps = 4;
	MarchingCubes.CubeSize = CubeSize;

	// 내부면 음수, 외부면 양수 리턴
	MarchingCubes.Implicit = Implicit;

	TArray<FVector3d> Seeds;
	for (int X = MarchingCubes.Bounds.Min.X; X <= MarchingCubes.Bounds.Max.X; X += SeedGap.X)
	{
		for (int Y = MarchingCubes.Bounds.Min.Y; Y <= MarchingCubes.Bounds.Max.Y; Y += SeedGap.Y)
		{
			for (int Z = MarchingCubes.Bounds.Min.Z; Z <= MarchingCubes.Bounds.Max.Z; Z += SeedGap.Z)
			{
				Seeds.Add(FVector3d(X, Y, Z));
			}
		}
	}

	MarchingCubes.GenerateContinuation(Seeds);

	// set uv, normal
	// normal과 uv 정보는 비어있어서 직접 설정
	{
		// set normal
		MarchingCubes.Normals.SetNum(MarchingCubes.Vertices.Num());
		for (int i = 0; i < MarchingCubes.Triangles.Num(); i++)
		{
			const int32 VID1 = MarchingCubes.Triangles[i].A;
			const int32 VID2 = MarchingCubes.Triangles[i].B;
			const int32 VID3 = MarchingCubes.Triangles[i].C;
			FVector3d EdgeA = MarchingCubes.Vertices[VID2] - MarchingCubes.Vertices[VID1];
			FVector3d EdgeB = MarchingCubes.Vertices[VID3] - MarchingCubes.Vertices[VID1];
			FVector3f TriangleNormal = FVector3f(EdgeB.Cross(EdgeA)).GetSafeNormal();
			
			MarchingCubes.Normals[VID1] += TriangleNormal;
			MarchingCubes.Normals[VID2] += TriangleNormal;
			MarchingCubes.Normals[VID3] += TriangleNormal;
		}
		for (auto& Normal : MarchingCubes.Normals)
		{
			Normal.Normalize();
		}

		// set uv
		MarchingCubes.UVs.SetNum(MarchingCubes.Vertices.Num());
		for (int32 i = 0; i < MarchingCubes.Vertices.Num(); i++)
		{
			const FVector& V = MarchingCubes.Vertices[i];
			const float U = FMath::Atan2(V.Y, V.X) / (2 * PI) + 0.5f;
			const float VCoord = FMath::Asin(V.Z / V.Size()) / PI + 0.5f;
			const FVector2f UV = FVector2f(U, VCoord);
			MarchingCubes.UVs[i] = UV;
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("MarchingCube Data Cnt - Vert : %d / norm : %d / uvs %d /  triuvs : %d / tri : %d / triNorm : %d"),
	//        MarchingCubes.Vertices.Num(), MarchingCubes.Normals.Num(),
	//        MarchingCubes.UVs.Num(), MarchingCubes.TriangleUVs.Num(),
	//        MarchingCubes.Triangles.Num(), MarchingCubes.TriangleNormals.Num());
}


FDynamicMesh3* AMarchingCubeWorld::CreateDynamicMesh(UE::Geometry::FMarchingCubes& MarchingCubes)
{
	// mesh set
	// FDynamicMesh3* Mesh = DynamicMeshComponent->GetMesh();
	FDynamicMesh3* Mesh = new FDynamicMesh3();

	Mesh->Clear();
	Mesh->EnableVertexNormals(FVector3f());
	Mesh->EnableVertexColors(FVector4f());
	Mesh->EnableAttributes();
	Mesh->Attributes()->EnablePrimaryColors();
	Mesh->Attributes()->EnableTangents();
	Mesh->EnableVertexUVs(FVector2f(0, 0));

	UE::Geometry::FDynamicMeshNormalOverlay* NormalOverlay = Mesh->Attributes()->PrimaryNormals();
	UE::Geometry::FDynamicMeshUVOverlay* UVOverlay = Mesh->Attributes()->PrimaryUV();

	if (MarchingCubes.Vertices.Num() != MarchingCubes.Normals.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("MC Vertcies, normals Num is not equal"));
		return nullptr;
	}

	for (int32 i = 0; i < MarchingCubes.Vertices.Num(); i++)
	{
		int32 VID = Mesh->AppendVertex(MarchingCubes.Vertices[i]);
		Mesh->SetVertexNormal(VID, MarchingCubes.Normals[i]);
	}

	for (auto UV : MarchingCubes.UVs)
	{
		UVOverlay->AppendElement(UV);
	}

	for (int i = 0; i < MarchingCubes.Triangles.Num(); i++)
	{
		int TriID = Mesh->AppendTriangle(MarchingCubes.Triangles[i]);
		if (i != TriID)
			UE_LOG(LogTemp, Warning, TEXT("Diff %d, %d"), i, TriID);

		NormalOverlay->SetTriangle(i, MarchingCubes.TriangleNormals[i], true);
		UVOverlay->SetTriangle(i, MarchingCubes.Triangles[i], true);
	}

	// UE_LOG(LogTemp, Log, TEXT("MC UVOverlay element cnt : %d"), UVOverlay->ElementCount());

	return Mesh;
}

// draw로 메시를 덧붙이려면 section을 추가하거나, 기존 section 다시 모두 계산해야하는 비효율 때문에 사용 x 
void AMarchingCubeWorld::CreateProceduralMesh(UE::Geometry::FMarchingCubes& MarchingCubes, UMaterial* Material, int32 SectionID)
{
	TArray<FVector> Vertices(MarchingCubes.Vertices);
	TArray<FVector2D> UVs(MarchingCubes.UVs);
	TArray<FVector> Normals(MarchingCubes.Normals);
	TArray<FProcMeshTangent> Tangents;

	TArray<int32> Triangles;
	for (auto Triangle : MarchingCubes.Triangles)
	{
		Triangles.Add(Triangle[0]);
		Triangles.Add(Triangle[1]);
		Triangles.Add(Triangle[2]);
	}

	UE_LOG(LogTemp, Log, TEXT("%d, %d, %d, %d"), Vertices.Num(), UVs.Num(), Normals.Num(), Triangles.Num());

	ProceduralMeshComponent->CreateMeshSection(SectionID, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
	if (Material)
	{
		ProceduralMeshComponent->SetMaterial(SectionID, Material);
	}
}

TFunction<double(UE::Math::TVector<double>)> AMarchingCubeWorld::ImplicitTerrain(float NoiseScale, float Height)
{
	return [&](UE::Math::TVector<double> Pt)
	{
		// 펄린노이즈 입력이 정수면 0을 리턴하기 때문에 0.1을 더함
		// return -1~ 1
		const float NoiseValue = FMath::PerlinNoise2D(FVector2d(Pt.X, Pt.Y) * NoiseScale + 0.1f);

		const double TerrainHeight = NoiseValue * Height;

		double Result = TerrainHeight - Pt.Z;

		return Result;
	};
}

TFunction<double(UE::Math::TVector<double>)> AMarchingCubeWorld::ImplicitSphere(float Radius, FVector Location)
{
	FVector LocalLocation = Location - GetActorLocation();

	return [&](UE::Math::TVector<double> Pt)
	{
		FVector Loc = Pt - LocalLocation;
		return Radius * Radius - Loc.SizeSquared();
	};
}


void AMarchingCubeWorld::Sculpt(FVector Location, FMarchingCubeSculptProperty& SculptProperty)
{
	UE::Geometry::FMarchingCubes MarchingCubes;

	const TFunction<double(UE::Math::TVector<double>)> Implicit = ImplicitSphere(SculptProperty.BrushRadius, Location);

	GenerateMarchingCubeData(MarchingCubes, Implicit,SculptProperty.SeedGap);
	FDynamicMesh3* SphereMesh = CreateDynamicMesh(MarchingCubes);

	UE::Geometry::FDynamicMeshEditor MeshEditor(DynamicMeshComponent->GetMesh());
	MeshEditor.AppendMesh(SphereMesh, IndexMappings);

	DynamicMeshComponent->NotifyMeshUpdated();
	DynamicMeshComponent->UpdateCollision(false);

}
