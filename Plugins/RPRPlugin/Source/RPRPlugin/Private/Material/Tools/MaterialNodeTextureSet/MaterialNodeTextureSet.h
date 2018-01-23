#pragma once

#include "RPRTypedefs.h"
#include "RPRUberMaterialParameters.h"

namespace RPR
{
	class FMaterialNodeTextureSet : public FGCObject
	{
	public:

		FMaterialNodeTextureSet() {}
		FMaterialNodeTextureSet(const FRPRUberMaterialParameters& UberMaterialParameters);

		virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	private:

		void	RegisterTextureFromProperty(const FRPRUberMaterialParameters& UberMaterialParameters, UProperty* Property);

		const FString&		GetXmlParamNameFromProperty(const UProperty* Property) const;
		bool				DoesPropertyUsingTexture(const FRPRUberMaterialParameters& UberMaterialParameters, const UProperty* Property) const;
		UTexture2D*			GetTextureProperty(const FRPRUberMaterialParameters& UberMaterialParameters, const UProperty* Property) const;

	private:

		TMap<FString, UTexture2D*>	TextureNodes;

	};
}
