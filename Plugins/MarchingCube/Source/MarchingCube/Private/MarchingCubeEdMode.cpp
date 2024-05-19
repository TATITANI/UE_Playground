// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCubeEdMode.h"


const FName FMarchingCubeEdMode::EM_MarchingCubeViewport = TEXT("EM_MarchingCubeViewport");


bool FMarchingCubeEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	return FEdMode::HandleClick(InViewportClient, HitProxy, Click);
}

bool FMarchingCubeEdMode::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
	// UE_LOG(LogTemp, Log, TEXT("MouseEnter"));
	return FEdMode::MouseEnter(ViewportClient, Viewport, x, y);
}

bool FMarchingCubeEdMode::MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport)
{
	// UE_LOG(LogTemp, Log, TEXT("MouseLeave"));
	return FEdMode::MouseLeave(ViewportClient, Viewport);
}

bool FMarchingCubeEdMode::InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta,
                                    float DeltaTime)
{
	if (bLockViewportMoving && bMouseLeftPress)
	{
		if (Key == EKeys::MouseX || Key == EKeys::MouseY)
		{
			return true;
		}
	}

	return FEdMode::InputAxis(InViewportClient, Viewport, ControllerId, Key, Delta, DeltaTime);
}

bool FMarchingCubeEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	// draw할 때 카메라 이동하지 않도록 잠금
	if (Key == EKeys::LeftMouseButton)
	{
		if (Event == IE_Pressed)
		{
			bMouseLeftPress = true;
		}
		else if (Event == IE_Released)
		{
			bMouseLeftPress = false;
		}
	}

	return FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

void FMarchingCubeEdMode::LockViewportMoving(bool bLock)
{
	bLockViewportMoving = bLock;
}
