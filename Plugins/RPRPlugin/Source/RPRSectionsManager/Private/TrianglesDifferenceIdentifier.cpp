#include "TrianglesDifferenceIdentifier.h"
#include "RPRSectionsManagerModeSettings.h"

FTrianglesDifferenceIdentifier::~FTrianglesDifferenceIdentifier()
{
	AbortAllTasks();
}

void FTrianglesDifferenceIdentifier::EnqueueNewTask(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
	TArray<uint32>& NewTriangles)
{
	check(SelectionFlags);

	auto newTask = 
		new FAsyncTask<FTriangleDiffAsyncTask>(
			MeshDataPtr,
			SelectionFlags,
			SelectionVisualizer,
			MoveTemp(NewTriangles));

	bool bShouldStart = Tasks.IsEmpty();
	Tasks.Enqueue(newTask);
	if (bShouldStart)
	{
		const auto settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
		if (settings->bAsynchronousSelection)
		{
			newTask->StartBackgroundTask();
		}
		else
		{
			newTask->StartSynchronousTask();
		}
	}
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

void FTrianglesDifferenceIdentifier::ExecuteTask(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
	TArray<uint32>& NewTriangles)
{
	check(SelectionFlags);
	FTriangleDiffAsyncTask task(MeshDataPtr, SelectionFlags, SelectionVisualizer, MoveTemp(NewTriangles));
	task.DoWork();
	SelectionVisualizer->UpdateIndicesRendering();
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::DoWork()
{
	if (bIsCancelled)
	{
		return;
	}

	QUICK_SCOPE_CYCLE_COUNTER(STAT_RPRSectionsManagerMode_GetNewRegisteredTrianglesAndIndices);
	
	URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
	const int32 BlockOfWorkPerFrame = settings->BlockOfWorkPerFrameForSelection;

	int32 start = NumTrianglesDone;
	int32 end = FMath::Min(start + BlockOfWorkPerFrame, NewTriangles.Num());

	for (int32 i = start; i < end; ++i)
	{
		const int32 newTriangleValue = NewTriangles[i];
		if (!SelectionFlags->IsTriangleUsed(newTriangleValue))
		{
			SelectionFlags->SetFlagAsUsed(newTriangleValue);
			SelectionVisualizer->SelectTriangle(newTriangleValue, false);
		}
	}

	NumTrianglesDone = end;
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::Abandon()
{
	bIsCancelled = true;
}
