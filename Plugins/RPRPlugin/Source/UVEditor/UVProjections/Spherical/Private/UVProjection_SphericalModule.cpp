#include "UVProjection_SphericalModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionSpherical.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "UVProjection_SphericalModule"

void FUVProjection_SphericalModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_SphericalModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_SphericalModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionSpherical)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_SphericalModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_Spherical", "Spherical"));
}

const FSlateBrush* FUVProjection_SphericalModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Sphere"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_SphericalModule, UVProjection_Spherical);