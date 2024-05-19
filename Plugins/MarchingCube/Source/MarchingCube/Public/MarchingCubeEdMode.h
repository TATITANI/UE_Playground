// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "UObject/ScriptDelegates.h"
#include "UObject/ObjectMacros.h"
/**
 * 
 */

class MARCHINGCUBE_API FMarchingCubeEdMode : public FEdMode
{
public:
	static const FName EM_MarchingCubeViewport;

	// 클릭 뗐을 때
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;

	// 마우스가 뷰포트 영역에 들어왔을때
	virtual bool MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;

	// 마우스가 뷰포트 영역에 나갔을 때
	virtual bool MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport) override;

	// 마우스 어떤 버튼이든 클릭한 상태로 이동
	virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;

	// 입력 발생
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;

private:
	bool bMouseLeftPress = false;
	bool bLockViewportMoving = false;
	
public:
	bool IsPressingMouseLeft() const {return bMouseLeftPress;}

	void LockViewportMoving(bool bLock);

};
