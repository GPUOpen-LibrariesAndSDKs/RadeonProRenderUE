#pragma once
#include "TriPlanarSettings/TriPlanarSettingsInterfaceEditor.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "TriPlanarSettingsObject.generated.h"

class RPREDITORTOOLS_API FTriPlanarSettingsCustomizationLayout : public IDetailCustomization
{
public:

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	void CustomizeTriPlanarObject(IDetailLayoutBuilder& DetailBuilder, class UTriPlanarSettingsObject* TriPlanarSettingsObject);
};

UCLASS()
class RPREDITORTOOLS_API UTriPlanarSettingsObject : public UObject
{
	GENERATED_BODY()
	
public:

	TArray<FTriPlanarSettingsInterfaceEditor> TriplanarSettingsInterfaces;

};
