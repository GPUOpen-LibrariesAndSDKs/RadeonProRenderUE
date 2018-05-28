#pragma once

#include "ObjectMacros.h"
#include "TriPlanarSettings.generated.h"

/**
 * Describes the settings of the tri planar mode
 */
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FTriPlanarSettings
{
	GENERATED_BODY()
	
public:

	FTriPlanarSettings();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool	bUseTriPlanar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	Angle;
	
};
