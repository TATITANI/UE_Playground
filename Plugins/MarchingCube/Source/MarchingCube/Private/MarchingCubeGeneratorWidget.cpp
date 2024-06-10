// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCubeGeneratorWidget.h"

#include "EditorModeManager.h"
#include "EngineUtils.h"
#include "MarchingCubeEdMode.h"
#include "MarchingCubeWorld.h"


void UMarchingCubeGeneratorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GLevelEditorModeTools().ActivateMode(FMarchingCubeEdMode::EM_MarchingCubeViewport);
	EdMode = static_cast<FMarchingCubeEdMode*>(GLevelEditorModeTools().GetActiveMode(FMarchingCubeEdMode::EM_MarchingCubeViewport));
	if (EdMode == nullptr)
	{
		Destruct();
		UE_LOG(LogTemp, Error, TEXT("MarchingCube EditorMode is nullptr"));
		return;
	}

	DrawBtnMap.Add(Btn_Menu_Generate, EDrawType::Generate);
	DrawBtnMap.Add(Btn_Menu_Sculpt, EDrawType::Sculpt);
	DrawBtnMap.Add(Btn_Menu_Erode, EDrawType::Erode);
	DrawBtnMap.GetKeys(DrawBtns);
	SetDrawType(EDrawType::Generate);

	CollectMCWorlds();
	OnActorAddedHandle = GEngine->OnLevelActorAdded().AddUObject(this, &UMarchingCubeGeneratorWidget::OnActorAdded);
	OnActorRemovedHandle = GEngine->OnLevelActorDeleted().AddUObject(this, &UMarchingCubeGeneratorWidget::OnActorDeleted);
}

void UMarchingCubeGeneratorWidget::NativeDestruct()
{
	UE_LOG(LogTemp, Log, TEXT("UMarchingCubeGeneratorWidget::NativeDestruct"));

	Super::NativeDestruct();

	GEngine->OnLevelActorAdded().Remove(OnActorAddedHandle);
	GEngine->OnLevelActorDeleted().Remove(OnActorAddedHandle);

	GLevelEditorModeTools().DeactivateMode(FMarchingCubeEdMode::EM_MarchingCubeViewport);
}

void UMarchingCubeGeneratorWidget::OnActorAdded(AActor* Actor)
{
	if (Actor->IsA(AMarchingCubeWorld::StaticClass()))
	{
		AMarchingCubeWorld* MCWorld = Cast<AMarchingCubeWorld>(Actor);
		MCWorlds.Add(MCWorld);
		UE_LOG(LogTemp,Log,TEXT("Add MCWorld Success"));

	}
}

void UMarchingCubeGeneratorWidget::OnActorDeleted(AActor* Actor)
{
	if (Actor->IsA(AMarchingCubeWorld::StaticClass()))
	{
		AMarchingCubeWorld* MCWorld = Cast<AMarchingCubeWorld>(Actor);
		if (MCWorlds.Contains(MCWorld))
		{
			MCWorlds.Remove(MCWorld);
			UE_LOG(LogTemp,Log,TEXT("Remove MCWorld Success"));
		}
	}
}


void UMarchingCubeGeneratorWidget::SpawnCubeWorld()
{
	AMarchingCubeWorld* MarchingCubeWorld = SpawnMarchingCubeWorld();
	MarchingCubeWorld->GenerateCubeWorld(MarchingCubeProperty, BoundSize);
}

void UMarchingCubeGeneratorWidget::SpawnTerrainWorld()
{
	AMarchingCubeWorld* MarchingCubeWorld = SpawnMarchingCubeWorld();
	MarchingCubeWorld->GenerateTerrainWorld(MarchingCubeProperty, BoundSize);
}

AMarchingCubeWorld* UMarchingCubeGeneratorWidget::SpawnMarchingCubeWorld()
{
	ensure(MarchingCubeWorldClass != nullptr);
	AMarchingCubeWorld* MarchingCubeWorld = GetWorld()->SpawnActor<AMarchingCubeWorld>(MarchingCubeWorldClass);
	MCWorlds.Add(MarchingCubeWorld);
	return MarchingCubeWorld;
}

void UMarchingCubeGeneratorWidget::OnClickDrawBtn(UButton* Button)
{
	if (DrawBtnMap.Contains(Button) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Button isn't MarchingCube Drawing Btn"));
		return;
	}

	const EDrawType DrawType = DrawBtnMap[Button];
	SetDrawType(DrawType);
}

void UMarchingCubeGeneratorWidget::SetDrawType(EDrawType DrawType)
{
	CurrentDrawType = DrawType;

	// 뷰포트 제어 잠금
	const bool bLockViewportMoving = CheckBrushMode(CurrentDrawType);
	EdMode->LockViewportMoving(bLockViewportMoving);

	for (auto DrawBtn : DrawBtns)
	{
		const bool bSelected = DrawBtnMap[DrawBtn] == CurrentDrawType;
		DrawBtn->SetBackgroundColor(bSelected ? SelectedColor : DeselectedColor);
	}
}

bool UMarchingCubeGeneratorWidget::CheckBrushMode(EDrawType DrawType)
{
	switch (DrawType)
	{
	case EDrawType::Sculpt:
	case EDrawType::Erode:
		return true;
	}

	return false;
}


void UMarchingCubeGeneratorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CheckBrushMode(CurrentDrawType) == false)
		return;

	AMarchingCubeWorld* HitMCWorld = nullptr;
	FVector BrushLocation, TraceWorldDir;
	const bool bTraceMCWorld = TraceBrush(HitMCWorld, BrushLocation, TraceWorldDir);
	if (!bTraceMCWorld)
		return;

	// draw brush
	const float BrushRadius = CurrentDrawType == EDrawType::Sculpt ? SculptProperty.BrushRadius : ErodeProperty.BrushRadius;
	DrawDebugSphere(GetWorld(), BrushLocation, BrushRadius, 32, FColor::Green, false);

	// 브러시 동작
	if (EdMode->IsPressingMouseLeft() == false)
	{
		return;
	}

	constexpr float BrushIntervalSec = 0.02f;
	AccumulatedTime += InDeltaTime;
	if (AccumulatedTime > BrushIntervalSec)
		AccumulatedTime = 0;
	else
		return;

	const uint64 StartTime = FPlatformTime::Cycles64();
	switch (CurrentDrawType)
	{
	case EDrawType::Sculpt:
		HitMCWorld->Sculpt(BrushLocation, SculptProperty);
		break;
	case EDrawType::Erode:
		// MarchingCubeWorld->ErodeLegacy(BrushLocation, ErodeProperty, TraceWorldDir);
		HitMCWorld->Erode(BrushLocation, ErodeProperty, TraceWorldDir);
		break;
	}

	const uint64 EndTime = FPlatformTime::Cycles64();
	const double ElapsedTime = FPlatformTime::ToSeconds64(EndTime - StartTime);
	UE_LOG(LogTemp, Log, TEXT("Draw ElapsedTime %f sec"), ElapsedTime);
}

void UMarchingCubeGeneratorWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
}


void UMarchingCubeGeneratorWidget::CollectMCWorlds()
{
	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
	if (!EditorWorld)
		return;
	for (TActorIterator<AMarchingCubeWorld> It(EditorWorld); It; ++It)
	{
		AMarchingCubeWorld* MCWorld = *It;
		if (MCWorld)
			MCWorlds.Add(MCWorld);
	}

	UE_LOG(LogTemp, Log, TEXT("MCWorlds num : %d"), MCWorlds.Num());
}


TOptional<FRay3d> UMarchingCubeGeneratorWidget::GetCursorRay()
{
	const bool bPlayLevel = GEditor->PlayWorld != nullptr;

	TOptional<FRay3d> Ray;
	if (GEditor && GEditor->GetActiveViewport() && !bPlayLevel)
	{
		FViewport* Viewport = GEditor->GetActiveViewport();
		FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(Viewport->GetClient());
		if (ViewportClient == nullptr)
			return Ray;

		FIntPoint MousePosition;
		Viewport->GetMousePos(MousePosition);

		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				ViewportClient->Viewport,
				ViewportClient->GetScene(),
				ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(ViewportClient->IsRealtime()));

		FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);

		FVector WorldMouseLocation, TraceWorldDir;
		if (SceneView)
		{
			SceneView->DeprojectFVector2D(MousePosition, WorldMouseLocation, TraceWorldDir);
		}

		const FVector TraceStartLoc = WorldMouseLocation;
		const FVector TraceEndLoc = WorldMouseLocation + TraceWorldDir * 9876543210;
		Ray = FRay3d(TraceStartLoc, TraceEndLoc);
	}

	return Ray;
}

bool UMarchingCubeGeneratorWidget::TraceBrush(AMarchingCubeWorld*& HitMCWorld, FVector& HitPoint, FVector& TraceWorldDir)
{
	auto CursorRay = GetCursorRay();
	TraceWorldDir = CursorRay->Direction;
	bool bTrace = false;

	if (ensureMsgf(CursorRay.IsSet(), TEXT("Cursor Ray invalid")))
	{
		double MinDistanceSquared = DBL_MAX;
		for (AMarchingCubeWorld* MCWorld : MCWorlds)
		{
			if (MCWorld == nullptr)
				continue;

			TOptional<FVector> CurrentHitPoint = MCWorld->GetRayHitPoint(CursorRay.GetValue());
			if (CurrentHitPoint.IsSet())
			{
				const double DistanceSquared = CursorRay->DistSquared(CurrentHitPoint.GetValue());
				if (DistanceSquared < MinDistanceSquared)
				{
					MinDistanceSquared = DistanceSquared;
					HitPoint = CurrentHitPoint.GetValue();

					HitMCWorld = MCWorld;
					bTrace = true;
				}
			}
		}
	}

	return bTrace;
}
