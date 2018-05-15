#pragma once
#include "AsyncWork.h"
#include "RPRMeshData.h"
#include "TrianglesSelectionFlags.h"

class FTrianglesDifferenceIdentifier
{
public:
	virtual ~FTrianglesDifferenceIdentifier();

	void EnqueueNewTask(
		const FRPRMeshDataPtr MeshDataPtr, 
		TSharedPtr<FTrianglesSelectionFlags> TriangleSelectionFlags, 
		const TArray<uint32>& NewTriangles, 
		const TArray<uint32>* MeshIndices, 
		TArray<uint32>* InOutRegisteredTriangles
	);

	bool IsLastTaskCompleted() const;
	const FRPRMeshDataPtr GetLastTaskRPRMeshData() const;
	const TArray<uint32>& GetLastTaskResult() const;
	void DequeueCompletedTask();
	bool HasTasks() const;
	void AbortAllTasks();

	static TArray<uint32> ExecuteTask(
		const FRPRMeshDataPtr MeshDataPtr,
		TSharedPtr<FTrianglesSelectionFlags> TriangleSelectionFlags,
		const TArray<uint32>& NewTriangles,
		const TArray<uint32>* MeshIndices,
		TArray<uint32>* InOutRegisteredTriangles);

private:

	class FTriangleDiffAsyncTask
	{
		friend class FAsyncTask<FTriangleDiffAsyncTask>;

	public:

		FRPRMeshDataPtr MeshDataPtr;
		TSharedPtr<FTrianglesSelectionFlags> TriangleSelectionFlags;
		const TArray<uint32>* NewTriangles;
		const TArray<uint32>* MeshIndices;
		TArray<uint32>* RegisteredTriangles;
		TArray<uint32> NewIndicesSelected;
		int32 TriangleOffset;
		int32 NumTrianglesDone;
		bool bIsInitialized;
		bool bIsTaskCompleted;
		bool bIsCancelled;

		FTriangleDiffAsyncTask()
			: NewTriangles(nullptr)
			, MeshIndices(nullptr)
			, TriangleOffset(0)
			, NumTrianglesDone(0)
			, bIsInitialized(false)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		FTriangleDiffAsyncTask(FRPRMeshDataPtr meshDataPtr, TSharedPtr<FTrianglesSelectionFlags> triangleSelectionFlags, const TArray<uint32>& newTriangles, const TArray<uint32>* meshIndices, TArray<uint32>* registeredTriangles)
			: MeshDataPtr(meshDataPtr)
			, TriangleSelectionFlags(triangleSelectionFlags)
			, NewTriangles(new TArray<uint32>(newTriangles))
			, MeshIndices(meshIndices)
			, RegisteredTriangles(registeredTriangles)
			, TriangleOffset(0)
			, NumTrianglesDone(0)
			, bIsInitialized(false)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		~FTriangleDiffAsyncTask()
		{
			delete NewTriangles;
		}

		void DoWork();
		void Abandon();
		
		FORCEINLINE bool CanAbandon() const { return (true); }
		FORCEINLINE TStatId GetStatId() const 
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(TriangleDiffAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
		}

		int32 FindByDichotomy(const TArray<uint32>& Array, uint32 Value, int32 Start, int32 End, int32& OutNearestIndex);

	};

	TQueue<FAsyncTask<FTriangleDiffAsyncTask>*> Tasks;
};
