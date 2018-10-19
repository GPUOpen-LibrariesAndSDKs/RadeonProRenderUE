#pragma once
#include "Engine/StaticMesh.h"
#include "Material/TriPlanarSettings.h"
#include "Containers/Map.h"
#include "Assets/RPRMaterial.h"
#include "MaterialEditor/DEditorParameterValue.h"

class RPREDITORTOOLS_API FTriPlanarSettingsInterfaceEditor
{

public:

	struct FTriPlanarData
	{
		FString GroupName;

		UDEditorParameterValue* UseTriPlanarParameter;
		UDEditorParameterValue* TriPlanarTextureAngleParameter;
		UDEditorParameterValue* TriPlanarTextureScaleParameter;

		FTriPlanarSettings Settings;
	};

	using FTriPlanarDataPerCategory = TMap<FString, FTriPlanarData>;
	using FTriPlanarDataPerMaterial = TMap<URPRMaterial*, FTriPlanarDataPerCategory>;
	using FTriPlanarDataPerMesh = TMap<UStaticMesh*, FTriPlanarDataPerMaterial>;

public:

	void	LoadMesh(UStaticMesh* StaticMesh);

	FTriPlanarDataPerMesh&	GetTriPlanarData();

private:

	void	LoadRPRMaterial(UStaticMesh* StaticMesh, URPRMaterial* Material);

	
private:

	FTriPlanarDataPerMesh	TriPlanarDataPerMesh;

};