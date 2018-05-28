#include "UVProjection_TriPlanarModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionTriPlanar.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "UVProjection_TriPlanarModule"

void FUVProjection_TriPlanarModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_TriPlanarModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_TriPlanarModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionTriPlanar)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_TriPlanarModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_TriPlanar", "TriPlanar"));
}

const FSlateBrush* FUVProjection_TriPlanarModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Plane"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_TriPlanarModule, UVProjection_TriPlanar);