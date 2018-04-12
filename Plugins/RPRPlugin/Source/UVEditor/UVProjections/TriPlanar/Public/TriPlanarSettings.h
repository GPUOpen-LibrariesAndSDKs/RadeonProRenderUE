// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ObjectMacros.h"
#include "TriPlanarSettings.generated.h"

/**
 * Describes the settings of the TriPlanar "projection"
 */
UCLASS()
class UVPROJECTION_TRIPLANAR_API UTriPlanarSettings : public UObject
{
	GENERATED_BODY()
	
public:

	UTriPlanarSettings();


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip="Uncheck to disable manually the tri planar if desired"), Category = TriPlanar)
	bool	bUseTriPlanar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TriPlanar)
	float	Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TriPlanar)
	float	Angle;
	
};
