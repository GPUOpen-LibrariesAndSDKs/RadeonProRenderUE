#include "UVProjection_CylindricalModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionCylinder.h"

#define LOCTEXT_NAMESPACE "UVProjection_CylindricalModule"

void FUVProjection_CylindricalModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_CylindricalModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_CylindricalModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionCylinder)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_CylindricalModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_Cylindrical", "Cylindrical"));
}

const FSlateBrush* FUVProjection_CylindricalModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Cylinder"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_CylindricalModule, UVProjection_Cylindrical);