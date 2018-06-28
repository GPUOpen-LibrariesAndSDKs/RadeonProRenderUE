/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
