#include "UVProjection_CubicModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"

#define LOCTEXT_NAMESPACE "UVProjection_CubicModule"

void FUVProjection_CubicModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_CubicModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_CubicModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionCubic)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_CubicModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_Cubic", "Cubic"));
}

const FSlateBrush* FUVProjection_CubicModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Cube"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_CubicModule, UVProjection_Cubic);