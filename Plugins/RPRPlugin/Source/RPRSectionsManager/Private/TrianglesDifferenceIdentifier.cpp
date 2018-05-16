#include "TrianglesDifferenceIdentifier.h"
#include "RPRSectionsManagerModeSettings.h"

FTrianglesDifferenceIdentifier::~FTrianglesDifferenceIdentifier()
{
	AbortAllTasks();
}

void FTrianglesDifferenceIdentifier::EnqueueAsyncSelection(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
	TArray<uint32>& NewTriangles)
{
	check(SelectionFlags);
	check(SelectionVisualizer);

	auto newTask = 
		new FAsyncTask<FTriangleDiffAsyncTask>(
			MeshDataPtr,
			SelectionFlags,
			SelectionVisualizer,
			MoveTemp(NewTriangles),
			true);

	EnqueueTaskAndStartIfRequired(newTask);
}

void FTrianglesDifferenceIdentifier::EnqueueAsyncDeselection(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags, 
	UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer, 
	TArray<uint32>& Triangles)
{
	check(SelectionFlags);
	check(SelectionVisualizer);

	auto newTask =
		new FAsyncTask<FTriangleDiffAsyncTask>(
			MeshDataPtr,
			SelectionFlags,
			SelectionVisualizer,
			MoveTemp(Triangles),
			false);

	EnqueueTaskAndStartIfRequired(newTask);
}

bool FTrianglesDifferenceIdentifier::IsLastTaskCompleted() const
{
	FAsyncTask<FTriangleDiffAsyncTask>* task;
	check(Tasks.Peek(task));
	return (task->IsDone());
}

const FRPRMeshDataPtr FTrianglesDifferenceIdentifier::GetLastTaskRPRMeshData() const
{
	FAsyncTask<FTriangleDiffAsyncTask>* task;
	check(Tasks.Peek(task));
	return (task->GetTask().MeshDataPtr);
}

void FTrianglesDifferenceIdentifier::DequeueCompletedTask()
{
	if (IsLastTaskCompleted())
	{
		FAsyncTask<FTriangleDiffAsyncTask>* task;
		Tasks.Dequeue(task);
		task->EnsureCompletion(true);
		delete task;
		task = nullptr;

		if (!Tasks.IsEmpty())
		{
			Tasks.Peek(task);
			task->StartBackgroundTask();
		}
	}
}

bool FTrianglesDifferenceIdentifier::HasTasks() const
{
	return (!Tasks.IsEmpty());
}

void FTrianglesDifferenceIdentifier::AbortAllTasks()
{
	if (!Tasks.IsEmpty())
	{
		FAsyncTask<FTriangleDiffAsyncTask>* task;
		while (!Tasks.IsEmpty() && Tasks.Peek(task))
		{
			task->Cancel();
			task->EnsureCompletion();
			delete task;
			Tasks.Pop();
		}
	}
}

void FTrianglesDifferenceIdentifier::SelectNewTriangles(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
	TArray<uint32>& NewTriangles, bool bSelect)
{
	check(SelectionFlags);
	FTriangleDiffAsyncTask task(MeshDataPtr, SelectionFlags, SelectionVisualizer, MoveTemp(NewTriangles), bSelect);
	task.DoWork();
	SelectionVisualizer->UpdateIndicesRendering();
}


void FTrianglesDifferenceIdentifier::EnqueueTaskAndStartIfRequired(FAsyncTask<FTriangleDiffAsyncTask>* Task)
{
	bool bShouldStart = Tasks.IsEmpty();
	Tasks.Enqueue(Task);
	if (bShouldStart)
	{
		const auto settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
		if (settings->bAsynchronousSelection)
		{
			Task->StartBackgroundTask();
		}
		else
		{
			Task->StartSynchronousTask();
		}
	}
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::DoWork()
{
	if (bIsCancelled)
	{
		return;
	}

	QUICK_SCOPE_CYCLE_COUNTER(STAT_RPRSectionsManagerMode_GetNewRegisteredTrianglesAndIndices);

	auto settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
	const int32 BlockOfWorkPerFrame = settings->BlockOfWorkPerFrameForSelection;

	int32 start = NumTrianglesDone;
	int32 end = FMath::Min(start + BlockOfWorkPerFrame, NewTriangles.Num());

	TFunction<void(uint32)> applyMethod;
	
	if (bIsSelecting)
	{
		applyMethod = [this](uint32 Triangle) { SelectTriangle(Triangle); };
	}
	else
	{
		applyMethod = [this](uint32 Triangle) { DeselectTriangle(Triangle); };
	}

	for (int32 i = start; i < end; ++i)
	{
		applyMethod(NewTriangles[i]);
	}

	NumTrianglesDone = end;
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::Abandon()
{
	bIsCancelled = true;
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::SelectTriangle(uint32 TriangleValue)
{
	if (!SelectionFlags->IsTriangleUsed(TriangleValue))
	{
		SelectionFlags->SetFlagAsUsed(TriangleValue);
		SelectionVisualizer->SelectTriangle(TriangleValue, false);
	}
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::DeselectTriangle(uint32 TriangleValue)
{
	if (SelectionFlags->IsTriangleUsed(TriangleValue))
	{
		SelectionFlags->SetFlagAsUnused(TriangleValue);
		SelectionVisualizer->DeselectTriangle(TriangleValue);
	}
}
