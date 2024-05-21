// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCubeGeneratorWidget.h"

#include "EditorModeManager.h"
#include "MarchingCubeEdMode.h"
#include "MarchingCubeWorld.h"
#include "Kismet/KismetSystemLibrary.h"

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
	DrawBtnMap.GetKeys(DrawBtns);
	SetDrawType(EDrawType::Generate);
	
	UE_LOG(LogTemp, Log, TEXT("UMarchingCubeGeneratorWidget::NativeConstruct"));
}

void UMarchingCubeGeneratorWidget::NativeDestruct()
{
	UE_LOG(LogTemp, Log, TEXT("UMarchingCubeGeneratorWidget::NativeDestruct"));

	Super::NativeDestruct();

	GLevelEditorModeTools().DeactivateMode(FMarchingCubeEdMode::EM_MarchingCubeViewport);
}


void UMarchingCubeGeneratorWidget::SpawnWorld()
{
	AMarchingCubeWorld* MarchingCubeWorld = GetWorld()->SpawnActor<AMarchingCubeWorld>(MarchingCubeWorldClass);
	MarchingCubeWorld->Init(MarchingCubeProperty, BoundSize);

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
		return true;
	}
	
	return false;
}


void UMarchingCubeGeneratorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FHitResult HitResult = MouseTraceToObject();
	if (HitResult.bBlockingHit == false)
		return;
	
	AMarchingCubeWorld* MarchingCubeWorld = Cast<AMarchingCubeWorld>(HitResult.GetActor());
	if (MarchingCubeWorld == nullptr)
		return;

	if (CheckBrushMode(CurrentDrawType))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, SculptProperty.BrushRadius, 32, FColor::Green, false);
	}

	if (EdMode->IsPressingMouseLeft() == false)
		return;

	constexpr float BrushIntervalSec = 0.02f;
	AccumulatedTime += InDeltaTime;
	if (AccumulatedTime > BrushIntervalSec)
		AccumulatedTime = 0;
	else
		return;

	
	switch (CurrentDrawType)
	{
	case EDrawType::Sculpt:
		MarchingCubeWorld->Sculpt(HitResult.Location, SculptProperty);
		break;
	}
}

void UMarchingCubeGeneratorWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
}


FHitResult UMarchingCubeGeneratorWidget::MouseTraceToObject()
{
	FHitResult HitResult;

	const bool bPlayLevel = GEditor->PlayWorld != nullptr;

	if (GEditor && GEditor->GetActiveViewport() && !bPlayLevel)
	{
		FViewport* Viewport = GEditor->GetActiveViewport();
		FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(Viewport->GetClient());
		if (ViewportClient == nullptr)
			return HitResult;

		FIntPoint MousePosition;
		Viewport->GetMousePos(MousePosition);

		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				ViewportClient->Viewport,
				ViewportClient->GetScene(),
				ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(ViewportClient->IsRealtime()));

		FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);

		FVector WorldMouseLocation, WorldDirection;

		if (SceneView)
		{
			SceneView->DeprojectFVector2D(MousePosition, WorldMouseLocation, WorldDirection);
		}

		const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), WorldMouseLocation, WorldMouseLocation + WorldDirection * 9876543210,
		                                                        TraceTypeQuery, true, {},
		                                                        EDrawDebugTrace::None, HitResult, true,
		                                                        FLinearColor::Green, FLinearColor::Red);

		// UE_LOG(LogTemp, Log, TEXT("MousePos %s / WorldMousePos %s / WorldDir %s / hit : %d"),
		//        *MousePosition.ToString(), *WorldMouseLocation.ToString(), *WorldDirection.ToString(), bHit);
	}

	return HitResult;
}
