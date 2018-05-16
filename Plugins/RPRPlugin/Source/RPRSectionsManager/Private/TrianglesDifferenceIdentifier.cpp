#include "TrianglesDifferenceIdentifier.h"
#include "RPRSectionsManagerModeSettings.h"

FTrianglesDifferenceIdentifier::~FTrianglesDifferenceIdentifier()
{
	AbortAllTasks();
}

void FTrianglesDifferenceIdentifier::EnqueueNewTask(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	TArray<uint32>& NewTriangles, 
	const TArray<uint32>* MeshIndices)
{
	check(SelectionFlags);

	auto newTask = 
		new FAsyncTask<FTriangleDiffAsyncTask>(
			MeshDataPtr,
			SelectionFlags,
			MoveTemp(NewTriangles), 
			MeshIndices);

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

const TArray<uint32>& FTrianglesDifferenceIdentifier::GetLastTaskResult() const
{
	FAsyncTask<FTriangleDiffAsyncTask>* task;
	check(Tasks.Peek(task));
	return (task->GetTask().NewIndicesSelected);
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

TArray<uint32> FTrianglesDifferenceIdentifier::ExecuteTask(
	const FRPRMeshDataPtr MeshDataPtr, 
	FTrianglesSelectionFlags* SelectionFlags,
	TArray<uint32>& NewTriangles, 
	const TArray<uint32>* MeshIndices)
{
	check(SelectionFlags);
	FTriangleDiffAsyncTask task(MeshDataPtr, SelectionFlags, MoveTemp(NewTriangles), MeshIndices);
	task.DoWork();
	return (task.NewIndicesSelected);
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

	const TArray<uint32>& meshIndices = *MeshIndices;

	int32 start = NumTrianglesDone;
	int32 end = FMath::Min(start + BlockOfWorkPerFrame, NewTriangles.Num());

	for (int32 i = start; i < end; ++i)
	{
		const int32 newTriangleValue = NewTriangles[i];
		if (!SelectionFlags->IsTriangleUsed(newTriangleValue))
		{
			SelectionFlags->SetFlagAsUsed(newTriangleValue);

			const int32 triangleIndexStart = newTriangleValue * 3;
			const int32 index = NewIndicesSelected.AddUninitialized(3);
			NewIndicesSelected[index] = meshIndices[triangleIndexStart];
			NewIndicesSelected[index + 1] = meshIndices[triangleIndexStart + 1];
			NewIndicesSelected[index + 2] = meshIndices[triangleIndexStart + 2];
		}
	}

	NumTrianglesDone = end;
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::Abandon()
{
	bIsCancelled = true;
}
