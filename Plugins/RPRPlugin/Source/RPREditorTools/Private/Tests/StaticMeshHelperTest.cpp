#include "AutomationTest.h"
#include "StaticMeshHelper.h"
#include "Engine/StaticMesh.h"
#include "RawMesh.h"

static void SetupPlaneOf4TrianglesSplitIn2Materials(TArray<uint32>& MeshIndices, TArray<int32>& MeshFaceMaterialIndices)
{
	MeshIndices.Add(0);
	MeshIndices.Add(1);
	MeshIndices.Add(2);

	MeshIndices.Add(1);
	MeshIndices.Add(3);
	MeshIndices.Add(2);

	MeshIndices.Add(2);
	MeshIndices.Add(3);
	MeshIndices.Add(4);

	MeshIndices.Add(3);
	MeshIndices.Add(5);
	MeshIndices.Add(4);

	MeshFaceMaterialIndices.Add(0);
	MeshFaceMaterialIndices.Add(0);
	MeshFaceMaterialIndices.Add(1);
	MeshFaceMaterialIndices.Add(1);
}

static void CheckIndices_PlaneOf4Triangles(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices)
{
	TArray<uint32> expectedMeshIndices;
	TArray<int32> meshFaceMaterialIndices;
	SetupPlaneOf4TrianglesSplitIn2Materials(expectedMeshIndices, meshFaceMaterialIndices);

	Test->TestEqual(TEXT("Check if the number of triangle has been altered"), MeshIndices.Num(), expectedMeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		Test->TestNotEqual(TEXT("Check degenerated triangle indices (0-1)"), MeshIndices[i], MeshIndices[i + 1]);
		Test->TestNotEqual(TEXT("Check degenerated triangle indices (1-2)"), MeshIndices[i + 1], MeshIndices[i + 2]);
	}
}

static void CheckTriangleOrder_PlaneOf4Triangles(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	TArray<uint32> originalMeshIndices;
	TArray<int32> meshFaceMaterialIndices;
	SetupPlaneOf4TrianglesSplitIn2Materials(originalMeshIndices, meshFaceMaterialIndices);

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], originalMeshIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i+1], originalMeshIndices[expectedTriangleIndex * 3+1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i+2], originalMeshIndices[expectedTriangleIndex * 3+2]);
	}
}

static void Setup3TrianglesSplitIn3Materials(TArray<uint32>& MeshIndices, TArray<int32>& MeshFaceMaterialIndices)
{
	MeshIndices.Add(0);
	MeshIndices.Add(1);
	MeshIndices.Add(2);

	MeshIndices.Add(0);
	MeshIndices.Add(3);
	MeshIndices.Add(4);

	MeshIndices.Add(0);
	MeshIndices.Add(4);
	MeshIndices.Add(1);

	MeshFaceMaterialIndices.Add(0);
	MeshFaceMaterialIndices.Add(1);
	MeshFaceMaterialIndices.Add(2);
}

static void Setup3TrianglesSplitIn2Materials(TArray<uint32>& MeshIndices, TArray<int32>& MeshFaceMaterialIndices)
{
	MeshIndices.Add(0);
	MeshIndices.Add(1);
	MeshIndices.Add(2);

	MeshIndices.Add(0);
	MeshIndices.Add(3);
	MeshIndices.Add(4);

	MeshIndices.Add(0);
	MeshIndices.Add(2);
	MeshIndices.Add(3);

	MeshFaceMaterialIndices.Add(0);
	MeshFaceMaterialIndices.Add(0);
	MeshFaceMaterialIndices.Add(1);
}

static void CheckTriangleOrder_3TrianglesSplitIn3Materials(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	TArray<uint32> originalMeshIndices;
	TArray<int32> meshFaceMaterialIndices;
	Setup3TrianglesSplitIn3Materials(originalMeshIndices, meshFaceMaterialIndices);

	Test->TestEqual(TEXT("Check original/new indices size"), originalMeshIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], originalMeshIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i + 1], originalMeshIndices[expectedTriangleIndex * 3 + 1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i + 2], originalMeshIndices[expectedTriangleIndex * 3 + 2]);
	}
}

static void CheckTriangleOrder_3TrianglesSplitIn2Materials(FAutomationTestBase* Test, const TArray<uint32>& MeshIndices, const TArray<int32>& ExpectedTriangleList)
{
	TArray<uint32> originalMeshIndices;
	TArray<int32> meshFaceMaterialIndices;
	Setup3TrianglesSplitIn2Materials(originalMeshIndices, meshFaceMaterialIndices);

	Test->TestEqual(TEXT("Check original/new indices size"), originalMeshIndices.Num(), MeshIndices.Num());

	for (int32 i = 0; i < MeshIndices.Num(); i += 3)
	{
		int32 expectedTriangleIndex = ExpectedTriangleList[i / 3];
		Test->TestEqual(TEXT("Check triangle order (0)"), MeshIndices[i], originalMeshIndices[expectedTriangleIndex * 3]);
		Test->TestEqual(TEXT("Check triangle order (1)"), MeshIndices[i + 1], originalMeshIndices[expectedTriangleIndex * 3 + 1]);
		Test->TestEqual(TEXT("Check triangle order (2)"), MeshIndices[i + 2], originalMeshIndices[expectedTriangleIndex * 3 + 2]);
	}
}

static void GenerateFakeRawMesh(FRawMesh& RawMesh, int32 NumFaces)
{
	RawMesh.VertexPositions.AddDefaulted(3);
	RawMesh.WedgeIndices.AddDefaulted(NumFaces * 3);
	RawMesh.WedgeTexCoords[0].AddDefaulted(NumFaces * 3);
	RawMesh.FaceSmoothingMasks.AddDefaulted(NumFaces);
	RawMesh.FaceMaterialIndices.AddDefaulted(NumFaces);
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

static void CheckTriangles(FAutomationTestBase* Test, const TArray<uint32>& OriginalMeshIndices, const TArray<uint32>& NewMeshIndices)
{
	Test->TestEqual(TEXT("Check triangles original/new size"), OriginalMeshIndices.Num(), NewMeshIndices.Num());

	for (int32 i = 0; i < OriginalMeshIndices.Num(); i += 3)
	{
		bool bHasFoundIdenticalTriangle = false;
		for (int32 j = 0; j < NewMeshIndices.Num(); j += 3)
		{
			if (NewMeshIndices[j] == OriginalMeshIndices[i] &&
				NewMeshIndices[j + 1] == OriginalMeshIndices[i + 1] &&
				NewMeshIndices[j + 2] == OriginalMeshIndices[i + 2])
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
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 0;

	SetupPlaneOf4TrianglesSplitIn2Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(2);
	SelectedTriangles.Add(3);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_FaceMaterial0_To_FaceMaterial1, 
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Face material 0 to material 1", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_FaceMaterial0_To_FaceMaterial1::RunTest(const FString& Parameters)
{
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	SetupPlaneOf4TrianglesSplitIn2Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_AllToFaceMaterial1,
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Remove face material 1",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_AllToFaceMaterial1::RunTest(const FString& Parameters)
{
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	SetupPlaneOf4TrianglesSplitIn2Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);
	SelectedTriangles.Add(3);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetFace0ToSection2,
	"RPR.StaticMeshHelper.AssignFacesToSelection.Plane_2Tri_2Mat.Set Face 0 to Mat 1",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetFace0ToSection2::RunTest(const FString& Parameters)
{
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 1;

	Setup3TrianglesSplitIn3Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(0);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetAllFacesToMat2,
	"RPR.StaticMeshHelper.AssignFacesToSection.3Triangles3Mat.Set All Faces To Mat 2",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles3Mat_SetAllFacesToMat2::RunTest(const FString& Parameters)
{
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 2;

	Setup3TrianglesSplitIn3Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

	return (true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStaticMeshHelperTest_AssignFacesToSection_3Triangles2Mat_SetAllFacesToMat0,
	"RPR.StaticMeshHelper.AssignFacesToSection.3Triangles2Mat.Set All Faces To Mat 0",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

	bool FStaticMeshHelperTest_AssignFacesToSection_3Triangles2Mat_SetAllFacesToMat0::RunTest(const FString& Parameters)
{
	TArray<int32> MeshFaceMaterialIndices;
	TArray<uint32> OriginalMeshIndices, MeshIndices;
	TArray<uint32> SelectedTriangles;
	int32 SectionIndex = 0;

	Setup3TrianglesSplitIn2Materials(MeshIndices, MeshFaceMaterialIndices);

	SelectedTriangles.Add(0);
	SelectedTriangles.Add(1);
	SelectedTriangles.Add(2);

	OriginalMeshIndices = MeshIndices;
	FStaticMeshHelper::AssignFacesToSection(MeshFaceMaterialIndices, MeshIndices, SelectedTriangles, SectionIndex);

	CheckSectionsContinuous(this, MeshFaceMaterialIndices);
	CheckTriangles(this, OriginalMeshIndices, MeshIndices);

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