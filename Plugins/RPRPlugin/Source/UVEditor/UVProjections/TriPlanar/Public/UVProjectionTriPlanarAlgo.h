#pragma once
#include "UVProjectionAlgorithmBase.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditHelper.h"

DECLARE_STATS_GROUP(TEXT("FUVProjectionTriPlanarAlgo"), STATGROUP_UVProjection_TriPlanarAlgo, STATCAT_Advanced)

class FUVProjectionTriPlanarAlgo : public FUVProjectionAlgorithmBase, public FGCObject
{
public:
	struct FSettings
	{
		FSettings();

		bool bApply;
		float Scale;
		float Angle;
	};
	

public:
	FUVProjectionTriPlanarAlgo();

	void SetSettings(const FSettings& InSettings);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual void StartAlgorithm() override;
	virtual void Finalize() override {}

private:

	void								SetMaterialParameterByMeshSection(UStaticMesh* StaticMesh, int32 SectionIndex);
	UMaterialEditorInstanceConstant*	CreateMaterialEditorInstanceConstant() const;
	TMap<FName, FMaterialParameterBrowseDelegate>	CreateEditMaterialParametersRouter() const;

	void	EditMaterialParameter_TriPlanar_TextureScale(UDEditorParameterValue* ParameterValue);
	void	EditMaterialParameter_TriPlanar_TextureAngle(UDEditorParameterValue* ParameterValue);

private:

	UMaterialEditorInstanceConstant*		MaterialEditorInstance;

	FSettings	Settings;
	
private:

	static const FName	MaterialParameterName_UseTriPlanar;
	static const FName	MaterialParameterName_TriPlanar_TextureScale;
	static const FName	MaterialParameterName_TriPlanar_TextureAngle;
};

typedef TSharedPtr<FUVProjectionTriPlanarAlgo>	FUVProjectionTriPlanarAlgoPtr;