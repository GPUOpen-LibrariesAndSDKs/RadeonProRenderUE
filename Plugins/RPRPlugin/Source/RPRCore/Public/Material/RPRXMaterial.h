#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRITypedefs.h"
#include "Typedefs/RPRXTypedefs.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/WeakObjectPtr.h"

class URPRMaterial;

namespace RPR
{

	class FRPRXMaterial
	{
	public:

		FRPRXMaterial();
		
		virtual ~FRPRXMaterial();

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

		TWeakObjectPtr<const URPRMaterial> UE4MaterialLink;

		TArray<RPR::FImagePtr> Images;
		RPRX::FMaterial	Material;

	};

}