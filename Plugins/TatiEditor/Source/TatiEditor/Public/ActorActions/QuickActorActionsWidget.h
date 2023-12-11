// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

/**
 * 
 */

UENUM()
enum class EDuplicationAxis : uint8
{
	XAxis UMETA(DisplayName = "X Axis"),
	YAxis UMETA(DisplayName = "Y Axis"),
	ZAxis UMETA(DisplayName = "Z Axis"),
	MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class TATIEDITOR_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActorSubsystem();

public:
	UFUNCTION(BlueprintCallable)
	void SelectSimilarNamedActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

#pragma  region ActorDuplication

	UFUNCTION(BlueprintCallable)
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "ActorBatchDuplication")
	EDuplicationAxis DuplicationAxis = EDuplicationAxis::XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "ActorBatchDuplication", meta=(ClampMin = 0))
	int32 NumberDuplication = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "ActorBatchDuplication")
	float OffsetDist = 300.0f;

#pragma  endregion

};
