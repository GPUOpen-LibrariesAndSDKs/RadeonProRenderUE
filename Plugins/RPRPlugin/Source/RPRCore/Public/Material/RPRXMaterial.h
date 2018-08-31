#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRITypedefs.h"
#include "Typedefs/RPRXTypedefs.h"
#include "UObject/GCObject.h"

class URPRMaterial;

namespace RPR
{

	class FRPRXMaterial : public FGCObject
	{
	public:

		FRPRXMaterial();
		
		virtual ~FRPRXMaterial();

		virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

		void				SetUE4MaterialLink(const URPRMaterial* UE4MaterialLink);
		const URPRMaterial*	GetUE4MaterialLink() const;

		void	AddImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImage Image);

		void	ReleaseResources();

		void				SetMaterial(RPRX::FMaterial InMaterial);
		RPRX::FMaterial		GetRawMaterial();
		RPRX::FMaterial		GetRawMaterial() const;
		

	private:

		const URPRMaterial* UE4MaterialLink;

		TArray<RPR::FImagePtr> Images;
		RPRX::FMaterial	Material;

	};

}