#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRITypedefs.h"
#include "Typedefs/RPRXTypedefs.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/WeakObjectPtr.h"
#include "RPRXEnums.h"

struct FRPRMaterialMap;
class URPRMaterial;

namespace RPR
{

	class RPRCORE_API FRPRXMaterial
	{
	public:
		FRPRXMaterial(const URPRMaterial* InUE4MaterialLink);
		virtual ~FRPRXMaterial();

		const URPRMaterial*	GetUE4MaterialLink() const;

		void	AddImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImage Image);

		RPR::FResult	SetMaterialParameterBool(RPRX::FParameter Parameter, bool Value);
		RPR::FResult	SetMaterialParameterUInt(RPRX::FParameter Parameter, uint32 Value);
		RPR::FResult	SetMaterialParameterColor(RPRX::FParameter Parameter, const FLinearColor& Color);
		RPR::FResult	SetMaterialParameterFloats(RPRX::FParameter Parameter, float x, float y, float z, float w);
		RPR::FResult	SetMaterialParameterFloat(RPRX::FParameter Parameter, float Value);
		RPR::FResult	SetMaterialParameterNode(RPRX::FParameter Parameter, RPR::FMaterialNode MaterialNode);
		RPR::FResult	Commit();

		void	ReleaseResources();

		/* 
		* Provide access to the native RPRX material pointer.
		* DO NOT DESTROY IT OR ANYTHING LIKE THAT.
		*/
		const RPRX::FMaterial&		GetRawMaterial() const;

		bool	IsMaterialValid() const;

	private:

		void	ReleaseRPRXMaterial();
		void	ReleaseMaterialNodes();
		void	ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap);
		void	ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode);		

		// Quick access to RPRX context
		RPRX::FContext	GetRprxContext() const;

	private:

		TWeakObjectPtr<const URPRMaterial> UE4MaterialLink;

		TArray<RPR::FImagePtr> Images;
		RPRX::FMaterial	Material;

	};

	typedef TSharedPtr<FRPRXMaterial> FRPRXMaterialPtr;

	// ----------------------------------------------------
	class RPRCORE_API FRPRXMaterialNode final
	{
	public:

		FRPRXMaterialNode(FString name, RPRX::EMaterialType type = RPRX::EMaterialType::Uber);		
		~FRPRXMaterialNode();

		void	AddImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImage Image);

		RPR::FResult	SetMaterialParameterBool(RPRX::FParameter Parameter, bool Value);
		RPR::FResult	SetMaterialParameterUInt(RPRX::FParameter Parameter, uint32 Value);
		RPR::FResult	SetMaterialParameterColor(RPRX::FParameter Parameter, const FLinearColor& Color);
		RPR::FResult	SetMaterialParameterFloats(RPRX::FParameter Parameter, float x, float y, float z, float w);
		RPR::FResult	SetMaterialParameterFloat(RPRX::FParameter Parameter, float Value);
		RPR::FResult	SetMaterialParameterNode(RPRX::FParameter Parameter, RPR::FMaterialNode MaterialNode);
		RPR::FResult	Commit();

		void	ReleaseResources();

		RPRX::FMaterial&		GetRawMaterial();

		bool	IsMaterialValid() const;

		// avoid copy material node
		FRPRXMaterialNode(const FRPRXMaterialNode&)            = delete;
		FRPRXMaterialNode& operator=(const FRPRXMaterialNode&) = delete;
	private:

		void	ReleaseRPRXMaterial();
		void	ReleaseMaterialNodes();
		void	ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap);
		void	ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode);		

		// Quick access to RPRX context
		RPRX::FContext	GetRprxContext() const;

		TArray<RPR::FImagePtr> Images;
		RPRX::FMaterial	Material;
		FString m_name;
		RPRX::EMaterialType m_type;
	};

	using FRPRXMaterialNodePtr = TSharedPtr<FRPRXMaterialNode>;

}