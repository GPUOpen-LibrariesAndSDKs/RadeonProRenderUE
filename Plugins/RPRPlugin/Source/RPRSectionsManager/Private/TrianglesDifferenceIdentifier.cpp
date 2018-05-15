#include "TrianglesDifferenceIdentifier.h"
#include "RPRSectionsManagerModeSettings.h"

FTrianglesDifferenceIdentifier::~FTrianglesDifferenceIdentifier()
{
	AbortAllTasks();
}

void FTrianglesDifferenceIdentifier::EnqueueNewTask(
	const FRPRMeshDataPtr MeshDataPtr, 
	TSharedPtr<FTrianglesSelectionFlags> TriangleSelectionFlags, 
	const TArray<uint32>& NewTriangles, 
	const TArray<uint32>* MeshIndices, 
	TArray<uint32>* InOutRegisteredTriangles)
{
	auto newTask = 
		new FAsyncTask<FTriangleDiffAsyncTask>(
			MeshDataPtr,
			TriangleSelectionFlags,
			NewTriangles, 
			MeshIndices,
			InOutRegisteredTriangles
			);

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
	TSharedPtr<FTrianglesSelectionFlags> TriangleSelectionFlags, 
	const TArray<uint32>& NewTriangles, 
	const TArray<uint32>* MeshIndices, 
	TArray<uint32>* InOutRegisteredTriangles)
{
	FTriangleDiffAsyncTask task(MeshDataPtr, TriangleSelectionFlags, NewTriangles, MeshIndices, InOutRegisteredTriangles);
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
	
	if (!bIsInitialized)
	{
		bIsInitialized = true;
		TriangleOffset = RegisteredTriangles->Num();
		RegisteredTriangles->Reserve(RegisteredTriangles->Num() + NewTriangles->Num());
		NewIndicesSelected.AddUninitialized(NewTriangles->Num() * 3);
	}

	URPRSectionsManagerModeSettings* settings = GetMutableDefault<URPRSectionsManagerModeSettings>();
	const int32 BlockOfWorkPerFrame = settings->BlockOfWorkPerFrameForSelection;

	TArray<uint32> newTriangles = *NewTriangles;
	newTriangles.Sort();

	const TArray<uint32>& meshIndices = *MeshIndices;

	int32 start = NumTrianglesDone;
	int32 end = FMath::Min(start + BlockOfWorkPerFrame, newTriangles.Num());

	int32 index = 0;
	int32 nearestIndex = 0;
	for (int32 i = start; i < end; ++i)
	{
		if (!TriangleSelectionFlags->IsTriangleUsed(newTriangles[i]))
		{
			TriangleSelectionFlags->SetFlagAsUsed(newTriangles[i]);
			RegisteredTriangles->Insert(newTriangles[i], nearestIndex);

			const int32 triangleIndexStart = newTriangles[i] * 3;
			NewIndicesSelected[i * 3] = meshIndices[triangleIndexStart];
			NewIndicesSelected[i * 3 + 1] = meshIndices[triangleIndexStart + 1];
			NewIndicesSelected[i * 3 + 2] = meshIndices[triangleIndexStart + 2];
		}
	}

	NumTrianglesDone = end;
}

int32 FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::FindByDichotomy(const TArray<uint32>& Array, uint32 Value, int32 Start, int32 End, int32& OutNearestIndex)
{
	if (Array.Num() == 0)
	{
		OutNearestIndex = 0;
		return (INDEX_NONE);
	}

	int32 mid = Start + (End - Start) / 2;
	OutNearestIndex = mid;

	if (Start == mid)
	{
		if (Array.IsValidIndex(Start) && Array[Start] == Value)
		{
			return (Start);
		}

		++OutNearestIndex;
		return (INDEX_NONE);
	}

	if (Array[mid] == Value)
	{
		return (mid);
	}
	else if (Array[mid] > Value)
	{
		return (FindByDichotomy(Array, Value, Start, mid, OutNearestIndex));
	}
	else
	{
		return (FindByDichotomy(Array, Value, mid, End, OutNearestIndex));
	}
}

void FTrianglesDifferenceIdentifier::FTriangleDiffAsyncTask::Abandon()
{
	bIsCancelled = true;
}
