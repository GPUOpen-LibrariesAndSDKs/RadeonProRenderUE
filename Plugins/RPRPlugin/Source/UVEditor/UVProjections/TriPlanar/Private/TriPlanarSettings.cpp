// Fill out your copyright notice in the Description page of Project Settings.

#include "TriPlanarSettings.h"
#include "Materials/MaterialInterface.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditHelper.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"

DECLARE_DELEGATE_OneParam(FSettingRouterDelegate, UDEditorParameterValue*);

UTriPlanarSettings::UTriPlanarSettings()
	: bUseTriPlanar(false)
	, Scale(100.0f)
	, Angle(0.0f)
{}

void UTriPlanarSettings::LoadFromMaterial(UMaterialInterface* MaterialInterface)
{
	TMap<FName, FSettingRouterDelegate> settingsRouter;
	if (settingsRouter.Num() == 0)
	{
		settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar, FSettingRouterDelegate::CreateUObject(this, &UTriPlanarSettings::LoadUseTriPlanar));
		settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale, FSettingRouterDelegate::CreateUObject(this, &UTriPlanarSettings::LoadScale));
		settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle, FSettingRouterDelegate::CreateUObject(this, &UTriPlanarSettings::LoadAngle));
	}

	FMaterialEditHelper::BindRouterAndExecute(MaterialInterface, settingsRouter);
}

void UTriPlanarSettings::LoadUseTriPlanar(UDEditorParameterValue* ParameterValue)
{
	UDEditorStaticSwitchParameterValue* staticSwitch = Cast<UDEditorStaticSwitchParameterValue>(ParameterValue);
	if (staticSwitch)
	{
		bUseTriPlanar = (staticSwitch->ParameterValue != 0);
	}
}

void UTriPlanarSettings::LoadScale(UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParam = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParam)
	{
		Scale = scalarParam->ParameterValue;
	}
}

void UTriPlanarSettings::LoadAngle(UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParam = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParam)
	{
		Angle = scalarParam->ParameterValue;
	}
}
