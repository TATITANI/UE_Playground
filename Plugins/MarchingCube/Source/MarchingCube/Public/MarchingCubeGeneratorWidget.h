// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "MarchingCubeEdMode.h"
#include "MarchingCubeWorld.h"
#include "Components/Button.h"

#include "MarchingCubeGeneratorWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EDrawType : uint8
{
	// None,
	Generate,
	Sculpt,
	Erode
};


UCLASS()
class MARCHINGCUBE_API UMarchingCubeGeneratorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Generate", meta=(AllowPrivateAccess))
	TSubclassOf<class AMarchingCubeWorld> MarchingCubeWorldClass;
	
	UPROPERTY(EditAnywhere, Category="Generate", meta=(AllowPrivateAccess))
	FVector3d BoundSize = FVector3d(500, 500, 1000);

	UPROPERTY(EditAnywhere, Category="Generate", meta=(AllowPrivateAccess))
	FMarchingCubeProperty MarchingCubeProperty;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	TEnumAsByte<ETraceTypeQuery> TraceTypeQuery;

	UPROPERTY(EditAnywhere, Category="Sculpt", meta=(AllowPrivateAccess))
	FMarchingCubeSculptProperty SculptProperty;

	UPROPERTY(EditAnywhere, Category="Erode", meta=(AllowPrivateAccess))
	FMarchingCubeErodeProperty ErodeProperty;
	
	float AccumulatedTime = 0;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidget))
	UButton* Btn_Menu_Generate;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidget))
	UButton* Btn_Menu_Sculpt;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidget))
	UButton* Btn_Menu_Erode;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	EDrawType CurrentDrawType = EDrawType::Generate;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	FLinearColor SelectedColor;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	FLinearColor DeselectedColor;

	TArray<UButton*> DrawBtns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TMap<UButton*, EDrawType> DrawBtnMap;

	FMarchingCubeEdMode* EdMode;

	
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

private:
	UFUNCTION(BlueprintCallable)
	void SpawnCubeWorld();

	UFUNCTION(BlueprintCallable)
	void SpawnTerrainWorld();
	
	UFUNCTION(BlueprintCallable)
	void OnClickDrawBtn(UButton* Button);

	void SetDrawType(EDrawType DrawType);
	bool CheckBrushMode(EDrawType DrawType);
	
	FHitResult MouseTraceToObject(FVector& TraceWorldDir);

};
