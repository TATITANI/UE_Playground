// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActionsWidget.h"

#include "DebugHeader.h"
#include "Subsystems/EditorActorSubsystem.h"

bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (EditorActorSubsystem == nullptr)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}

void UQuickActorActionsWidget::SelectSimilarNamedActors()
{
	if (GetEditorActorSubsystem() == false)
		return;

	uint32 SelectionCount = 0;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	for (auto SelectedActor : SelectedActors)
	{
		FString ActorName = SelectedActor->GetActorLabel();
		const FString NameSearching = ActorName.LeftChop(4); // 오른쪽 4글자 제거한 문자열 ex) abcdef -> ab
		TArray<AActor*> AllLevelActors = EditorActorSubsystem->GetAllLevelActors();

		for (AActor* ActorInLevel : AllLevelActors)
		{
			if (ActorInLevel == nullptr)
				continue;

			if (ActorInLevel->GetActorLabel().Contains(NameSearching, SearchCase))
			{
				// 레벨에디터에서 이름이 유사한 액터들 선택
				EditorActorSubsystem->SetActorSelectionState(ActorInLevel, true);
				SelectionCount++;
			}
		}
	}

	if (SelectionCount > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Success selected ") + FString::FromInt(SelectionCount) + TEXT(" actors"));
	}
	else
	{
		DebugHeader::ShowNotifyInfo(TEXT("no actor with similar name found"));
	}
}

#pragma  region ActorDuplication

void UQuickActorActionsWidget::DuplicateActors()
{
	if (!GetEditorActorSubsystem())
		return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 DuplicationCnt = 0;

	if (SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if (OffsetDist == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Invalid Duplication Distance"));
		return;
	}

	for (AActor* SelectedActor : SelectedActors)
	{
		if (SelectedActor == nullptr)
			continue;

		for (int32 i = 0; i < NumberDuplication; i++)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(SelectedActor, SelectedActor->GetWorld());
			if (DuplicatedActor == nullptr)
				continue;

			const float DuplicationOffsetDist = (i + 1) * OffsetDist;

			const FVector DuplicationOffset = DuplicationOffsetDist * FVector(DuplicationAxis == EDuplicationAxis::XAxis,
			                                                            DuplicationAxis == EDuplicationAxis::YAxis,
			                                                            DuplicationAxis == EDuplicationAxis::ZAxis);

			DuplicatedActor->AddActorWorldOffset(DuplicationOffset);

			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			DuplicationCnt++;
		}

		if(DuplicationCnt>0)
		{
			DebugHeader::ShowNotifyInfo(FString::Printf(TEXT("Successfully duplicated %d Actors"), DuplicationCnt));
		}
	}
}


#pragma  endregion

