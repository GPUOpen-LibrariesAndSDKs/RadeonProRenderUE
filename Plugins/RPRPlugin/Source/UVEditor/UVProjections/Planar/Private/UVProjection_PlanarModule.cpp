#include "UVProjection_PlanarModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionPlanar.h"

#define LOCTEXT_NAMESPACE "UVProjection_PlanarModule"

void FUVProjection_PlanarModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_PlanarModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_PlanarModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionPlanar)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_PlanarModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_Planar", "Planar"));
}

const FSlateBrush* FUVProjection_PlanarModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Plane"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_PlanarModule, UVProjection_Planar);