#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Templates/SharedPointer.h"
#include "Engine/Texture.h"

namespace RPR
{
	namespace GLTF
	{

		template<typename UE4Type, typename NativeType>
		class FImportResources
		{
		public:

			template<typename UE4Type, typename NativeType>
			struct TResourceData
			{
				int32 Id;
				UE4Type ResourceUE4;
				NativeType ResourceRPR;

				TResourceData(int32 id) 
					: Id(id)
					, ResourceUE4(nullptr)
					, ResourceRPR(nullptr)
				{}
			};

			using FResourceData = TResourceData<UE4Type, NativeType>;

		public:

			FResourceData&	RegisterNewResource(int32 id)
			{
				int32 index = ResourceDatas.Emplace(id);
				return (ResourceDatas[index]);
			}

			int32			GetNumResources() const
			{
				return (ResourceDatas.Num());
			}

			FResourceData*	FindResourceById(int32 id)
			{
				for (int32 i = 0; i < ResourceDatas.Num(); ++i)
				{
					if (ResourceDatas[i].Id == id)
					{
						return (&ResourceDatas[i]);
					}
				}
				return (nullptr);
			}

			FResourceData*	GetResource(int32 index)
			{
				return (ResourceDatas[index]);
			}

		protected:

			FResourceData*	FindResourceByUE4Type(UE4Type ResourceUE4)
			{
				for (int32 i = 0; i < ResourceDatas.Num(); ++i)
				{
					if (ResourceDatas[i].ResourceUE4 == ResourceUE4)
					{
						return (&ResourceDatas[i]);
					}
				}
				return (nullptr);
			}

			FResourceData*	FindResourceByNativeType(NativeType ResourceRPR)
			{
				for (int32 i = 0; i < ResourceDatas.Num(); ++i)
				{
					if (ResourceDatas[i].ResourceRPR == ResourceRPR)
					{
						return (&ResourceDatas[i]);
					}
				}
				return (nullptr);
			}

		private:

			TArray<FResourceData> ResourceDatas;

		};

	}
}