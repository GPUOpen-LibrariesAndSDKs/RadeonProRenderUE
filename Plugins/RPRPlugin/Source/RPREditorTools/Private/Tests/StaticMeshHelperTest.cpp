#include "AutomationTest.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "RawMesh.h"

static void GenerateFakeRawMesh(FRawMesh& RawMesh, int32 NumFaces)
{
	RawMesh.VertexPositions.AddDefaulted(3);
	RawMesh.WedgeIndices.AddDefaulted(NumFaces * 3);
	RawMesh.WedgeTexCoords[0].AddDefaulted(NumFaces * 3);
	RawMesh.FaceSmoothingMasks.AddDefaulted(NumFaces);
	RawMesh.FaceMaterialIndices.AddDefaulted(NumFaces);
}

static void SetupPlaneOf4TrianglesSplitIn2Materials(FRawMesh& RawMesh)
{
	GenerateFakeRawMesh(RawMesh, 4);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(1);
	RawMesh.WedgeIndices.Add(2);

	RawMesh.WedgeIndices.Add(1);
	RawMesh.WedgeIndices.Add(3);
	RawMesh.WedgeIndices.Add(2);

	RawMesh.WedgeIndices.Add(2);
	RawMesh.WedgeIndices.Add(3);
	RawMesh.WedgeIndices.Add(4);

	RawMesh.WedgeIndices.Add(3);
	RawMesh.WedgeIndices.Add(5);
	RawMesh.WedgeIndices.Add(4);

	RawMesh.FaceMaterialIndices.Add(0);
	RawMesh.FaceMaterialIndices.Add(0);
	RawMesh.FaceMaterialIndices.Add(1);
	RawMesh.FaceMaterialIndices.Add(1);
}

static void CheckIndices_PlaneOf4Triangles(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices)
{
	FRawMesh rawMesh;
	SetupPlaneOf4TrianglesSplitIn2Materials(rawMesh);

	Test->TestEqual(TEXT("Check original/new indices size"), rawMesh.WedgeIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		Test->TestNotEqual(TEXT("Check degenerated triangle indices (0-1)"), MeshIndices[i], MeshIndices[i + 1]);
		Test->TestNotEqual(TEXT("Check degenerated triangle indices (1-2)"), MeshIndices[i + 1], MeshIndices[i + 2]);
	}
}

static void CheckTriangleOrder_PlaneOf4Triangles(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	FRawMesh rawMesh;
	SetupPlaneOf4TrianglesSplitIn2Materials(rawMesh);

	Test->TestEqual(TEXT("Check original/new indices size"), rawMesh.WedgeIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], rawMesh.WedgeIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i+1], rawMesh.WedgeIndices[expectedTriangleIndex * 3+1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i+2], rawMesh.WedgeIndices[expectedTriangleIndex * 3+2]);
	}
}

static void Setup3TrianglesSplitIn3Materials(FRawMesh& RawMesh)
{
	GenerateFakeRawMesh(RawMesh, 3);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(1);
	RawMesh.WedgeIndices.Add(2);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(3);
	RawMesh.WedgeIndices.Add(4);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(4);
	RawMesh.WedgeIndices.Add(1);

	RawMesh.FaceMaterialIndices.Add(0);
	RawMesh.FaceMaterialIndices.Add(1);
	RawMesh.FaceMaterialIndices.Add(2);
}

static void Setup3TrianglesSplitIn2Materials(FRawMesh& RawMesh)
{
	GenerateFakeRawMesh(RawMesh, 3);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(1);
	RawMesh.WedgeIndices.Add(2);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(3);
	RawMesh.WedgeIndices.Add(4);

	RawMesh.WedgeIndices.Add(0);
	RawMesh.WedgeIndices.Add(2);
	RawMesh.WedgeIndices.Add(3);

	RawMesh.FaceMaterialIndices.Add(0);
	RawMesh.FaceMaterialIndices.Add(0);
	RawMesh.FaceMaterialIndices.Add(1);
}

static void CheckTriangleOrder_3TrianglesSplitIn3Materials(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	FRawMesh rawMesh;
	Setup3TrianglesSplitIn3Materials(rawMesh);

	Test->TestEqual(TEXT("Check original/new indices size"), rawMesh.WedgeIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], rawMesh.WedgeIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i + 1], rawMesh.WedgeIndices[expectedTriangleIndex * 3 + 1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i + 2], rawMesh.WedgeIndices[expectedTriangleIndex * 3 + 2]);
	}
}

static void CheckTriangleOrder_3TrianglesSplitIn2Materials(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	FRawMesh rawMesh;
	Setup3TrianglesSplitIn2Materials(rawMesh);

	Test->TestEqual(TEXT("Check original/new indices size"), rawMesh.WedgeIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], rawMesh.WedgeIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i + 1], rawMesh.WedgeIndices[expectedTriangleIndex * 3 + 1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i + 2], rawMesh.WedgeIndices[expectedTriangleIndex * 3 + 2]);
	}
}

static void CheckSectionsContinuous(FAutomationTestBase* Test, const TArray<int32>& MeshFaceMaterialIndices)
{
	int32 currentSection = MeshFaceMaterialIndices[0];

	for (int32 i = 1; i < MeshFaceMaterialIndices.Num(); ++i)
	{
		Test->TestFalse(TEXT("Check section continuous"), currentSection > MeshFaceMaterialIndices[i]);
		if (currentSection != MeshFaceMaterialIndices[i])
		{
			currentSection = MeshFaceMaterialIndices[i];
		}
	}
}

static void CheckTriangles(FAutomationTestBase* Test, const FRawMesh& OriginalRawMesh, const FRawMesh& NewRawMesh)
{
	const TArray<uint32>& originalMeshIndices = OriginalRawMesh.WedgeIndices;
	const TArray<uint32>& newMeshIndices = NewRawMesh.WedgeIndices;

	Test->TestEqual(TEXT("Check triangles original/new size"), originalMeshIndices.Num(), newMeshIndices.Num());

	for (int32 i = 0; i < originalMeshIndices.Num(); i += 3)
	{
		bool bHasFoundIdenticalTriangle = false;
		for (int32 j = 0; j < newMeshIndices.Num(); j += 3)
		{
			if (newMeshIndices[j] == originalMeshIndices[i] &&
				newMeshIndices[j + 1] == originalMeshIndices[i + 1] &&
				newMeshIndices[j + 2] == originalMeshIndices[i + 2])
			{
				bHasFoundIdenticalTriangle = true;
				break;
			}
		}

		Test->TestTrue(*FString::Printf(TEXT("CHeck triangle %d existence"), i), bHasFoundIdenticalTriangle);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_ConvertFaceMaterial1l_To_FaceMaterial0, 
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Convert face material 1 to face material 0", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_ConvertFaceMaterial1l_To_FaceMaterial0::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 0;

	FRawMesh rawMesh;
	SetupPlaneOf4TrianglesSplitIn2Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(2);
	SelectedTriangles.Add(3);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_FaceMaterial0_To_FaceMaterial1, 
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Face material 0 to material 1", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_FaceMaterial0_To_FaceMaterial1::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	FRawMesh rawMesh;
	SetupPlaneOf4TrianglesSplitIn2Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);
	
	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_AllToFaceMaterial1,
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Remove face material 1",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_AllToFaceMaterial1::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	FRawMesh rawMesh;
	SetupPlaneOf4TrianglesSplitIn2Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);
	SelectedTriangles.Add(3);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetFace0ToSection2,
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Set Face 0 to Mat 1",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetFace0ToSection2::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	FRawMesh rawMesh;
	Setup3TrianglesSplitIn3Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(0);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetAllFacesToMat2,
	"RPR.StaticMeshHelper.AssignFacesToSection.3Triangles3Mat.Set All Faces To Mat 2",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetAllFacesToMat2::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 2;

	FRawMesh rawMesh;
	Setup3TrianglesSplitIn3Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles2Mat_SetAllFacesToMat0,
	"RPR.StaticMeshHelper.AssignFacesToSection.3Triangles2Mat.Set All Faces To Mat 0",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

	bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles2Mat_SetAllFacesToMat0::RunTest(const FString& Parameters)
{
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 0;

	FRawMesh rawMesh;
	Setup3TrianglesSplitIn2Materials(rawMesh);
	FRawMesh originalRawMesh = rawMesh;

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);

	FStaticMeshHelper::AssignFacesToSection(rawMesh, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, rawMesh.FaceMaterialIndices);
	CheckTriangles(this, originalRawMesh, rawMesh);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_FindUnusedSections_Test012,
	"RPR.StaticMeshHelper.FindUnusedSections.Test012",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_FindUnusedSections_Test012::RunTest(const FString& Parameters)
{
	TArray<int32> missingSections;
	FStaticMeshHelper::FindUnusedSections({ 0, 1, 2 }, missingSections);
	TestEqual(TEXT("Check missing sections quantity"), missingSections.Num(), 0);
	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_FindUnusedSections_Test002,
	"RPR.StaticMeshHelper.FindUnusedSections.Test002",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_FindUnusedSections_Test002::RunTest(const FString& Parameters)
{
	TArray<int32> missingSections;
	FStaticMeshHelper::FindUnusedSections({ 0, 0, 2}, missingSections);
	TestEqual(TEXT("Check missing sections quantity"), missingSections.Num(), 1);
	TestEqual(TEXT("Check missed section"), missingSections[0], 1);
	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_FindUnusedSections_Test11,
	"RPR.StaticMeshHelper.FindUnusedSections.Test11",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

	bool FStaticMeshHelperTest_FindUnusedSections_Test11::RunTest(const FString& Parameters)
{
	TArray<int32> missingSections;
	FStaticMeshHelper::FindUnusedSections({ 1, 1 }, missingSections);
	TestEqual(TEXT("Check missing sections quantity"), missingSections.Num(), 1);
	TestEqual(TEXT("Check missed section"), missingSections[0], 0);
	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_FindUnusedSections_Test001,
	"RPR.StaticMeshHelper.FindUnusedSections.Test001",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

	bool FStaticMeshHelperTest_FindUnusedSections_Test001::RunTest(const FString& Parameters)
{
	TArray<int32> missingSections;
	FStaticMeshHelper::FindUnusedSections({ 0, 0, 1 }, missingSections);
	TestEqual(TEXT("Check missing sections quantity"), missingSections.Num(), 0);
	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_CleanUnusedMeshSections_3Triangles_3Mat,
	"RPR.StaticMeshHelper.CleanUnusedMeshSections.3Triangles_3Mat",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_CleanUnusedMeshSections_3Triangles_3Mat::RunTest(const FString& Parameters)
{
	const int32 lodIndex = 0;

	FRawMesh rawMesh;
	GenerateFakeRawMesh(rawMesh, 3);
	rawMesh.FaceMaterialIndices = TArray<int32>({ 1, 1, 2 });
	
	FMeshSectionInfoMap sectionInfoMap;
	{
		sectionInfoMap.Set(lodIndex, 0, FMeshSectionInfo(2));
		sectionInfoMap.Set(lodIndex, 1, FMeshSectionInfo(0));
		sectionInfoMap.Set(lodIndex, 2, FMeshSectionInfo(1));
	}

	TArray<FStaticMaterial> staticMaterials;
	{
		staticMaterials.Emplace(nullptr, "Red");
		staticMaterials.Emplace(nullptr, "Cyan");
		staticMaterials.Emplace(nullptr, "Blue");
	}

	FStaticMeshHelper::CleanUnusedMeshSections(rawMesh, sectionInfoMap, staticMaterials);

	TestEqual(TEXT("Check face material indices (0)"), rawMesh.FaceMaterialIndices[0], 0);
	TestEqual(TEXT("Check face material indices (1)"), rawMesh.FaceMaterialIndices[1], 0);
	TestEqual(TEXT("Check face material indices (2)"), rawMesh.FaceMaterialIndices[2], 1);

	TestEqual(TEXT("Check section info map size"), sectionInfoMap.GetSectionNumber(lodIndex), 2);
	TestEqual(TEXT("Check static materials array size"), staticMaterials.Num(), 2);

	TestTrue(TEXT("Check static material content (0)"), staticMaterials[0].MaterialSlotName == "Red");
	TestTrue(TEXT("Check static material content (1)"), staticMaterials[1].MaterialSlotName == "Cyan");

	TestTrue(TEXT("Check section info map content (0)"), sectionInfoMap.Get(lodIndex, 0).MaterialIndex == 0);
	TestTrue(TEXT("Check section info map content (0)"), sectionInfoMap.Get(lodIndex, 1).MaterialIndex == 1);

	return (true);
}