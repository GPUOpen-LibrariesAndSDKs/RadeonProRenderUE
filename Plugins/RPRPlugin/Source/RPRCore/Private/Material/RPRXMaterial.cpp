#include "Material/RPRXMaterial.h"
#include "Assets/RPRMaterial.h"

RPR::FRPRXMaterial::FRPRXMaterial()
	: Material(nullptr)
{}

RPR::FRPRXMaterial::~FRPRXMaterial()
{
	ReleaseResources();
}

void RPR::FRPRXMaterial::SetUE4MaterialLink(const URPRMaterial* InUE4MaterialLink)
{
	UE4MaterialLink = InUE4MaterialLink;
}

const URPRMaterial* RPR::FRPRXMaterial::GetUE4MaterialLink() const
{
	return UE4MaterialLink;
}

void RPR::FRPRXMaterial::AddImage(RPR::FImagePtr Image)
{
	Images.Add(Image);
}

void RPR::FRPRXMaterial::RemoveImage(RPR::FImagePtr Image)
{
	Images.Remove(Image);
}

void RPR::FRPRXMaterial::RemoveImage(RPR::FImage Image)
{
	Images.RemoveAll([Image] (RPR::FImagePtr imagePtr)
	{
		return imagePtr.IsValid() && (imagePtr.Get() == Image);
	});
}

void RPR::FRPRXMaterial::ReleaseResources()
{
	Images.Empty();
}

void RPR::FRPRXMaterial::SetMaterial(RPRX::FMaterial InMaterial)
{
	Material = InMaterial;
}

RPRX::FMaterial RPR::FRPRXMaterial::GetRawMaterial()
{
	return Material;
}

void RPR::FRPRXMaterial::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(UE4MaterialLink);
}

RPRX::FMaterial RPR::FRPRXMaterial::GetRawMaterial() const
{
	return Material;
}
