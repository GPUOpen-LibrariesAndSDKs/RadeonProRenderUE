#include "Miscs/RPRMaterialNodeDumper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRErrorsHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialNodeDump, Log, All);

namespace RPR
{
	namespace RPRMaterial
	{
		RPR::FResult DumpInputUint(const FString& Name, const FString& Idententation, RPR::FMaterialNode MaterialNode, int32 InputIndex)
		{
			RPR::FResult status;
			uint32 value;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, InputIndex, value);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> uint : Cannot get value"), *Idententation, *Name);
				return (status);
			}
			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> uint : [%d]"), *Idententation, *Name, value);
			return (status);
		}

		RPR::FResult DumpInputImage(const FString& Name, const FString& Idententation, RPR::FMaterialNode MaterialNode, int32 InputIndex)
		{
			RPR::FResult status;
			RPR::FImage image = nullptr;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, InputIndex, image);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Image : Cannot get image"), *Idententation, *Name);
				return (status);
			}
			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Image : [%p]"), *Idententation, *Name, image);
			return (status);
		}

		RPR::FResult DumpInputFloat4(const FString& Name, const FString& Idententation, RPR::FMaterialNode MaterialNode, int32 InputIndex)
		{
			RPR::FResult status;
			FLinearColor value;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, InputIndex, value);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Float : Cannot get value"), *Idententation, *Name);
				return (status);
			}
			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Float : [%f, %f, %f, %f]"), *Idententation, *Name, value.R, value.G, value.B, value.A);
			return (status);
		}

		RPR::FResult DumpNodeType(const FString& Idententation, RPR::FMaterialNode MaterialNode)
		{
			RPR::FResult status;
			RPR::EMaterialNodeType materialNodeType;
			status = RPR::RPRMaterial::GetNodeInfo(MaterialNode, RPR::EMaterialNodeInfo::Type, &materialNodeType);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sCannot get node type info (%d)"), *Idententation, status);
				return (status);
			}

			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%s> Type : %d"), *Idententation, (uint32) materialNodeType);
			return (status);
		}

		RPR::FResult InternalDumpMaterialNode(RPR::FContext Context, RPR::FMaterialNode MaterialNode, int32 Depth, RPRX::FContext RPRXContext)
		{
			RPR::FResult status;

			FString indentation = FString::ChrN(Depth, '-');
			FString materialHeader = FString::Printf(TEXT("%sMaterial Node [%p]"), *indentation, MaterialNode);

			if (MaterialNode != nullptr && RPRXContext != nullptr)
			{
				bool bIsMaterialX;
				status = RPRX::FMaterialHelpers::IsMaterialRPRX(RPRXContext, MaterialNode, bIsMaterialX);
				if (RPR::IsResultFailed(status))
				{
					materialHeader += TEXT(" [Unkown nature]");
				}
				else
				{
					materialHeader += bIsMaterialX ? TEXT(" [MaterialX]") : TEXT(" [Material]");
				}
			}

			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%s"), *materialHeader);

			if (MaterialNode == nullptr)
			{
				return RPR_SUCCESS;
			}

			status = DumpNodeType(indentation, MaterialNode);
			if (RPR::IsResultFailed(status))
			{
				RPR::Error::LogLastError(Context);
				 return status;
			}

			uint64 numInputs = 0;
			status = RPR::RPRMaterial::GetNodeInfo(MaterialNode, RPR::EMaterialNodeInfo::InputCount, &numInputs);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sCannot get node input count"), *indentation);
				return (status);
			}

			UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%s> Num inputs : %d"), *indentation, numInputs);

			for (int32 inputIndex = 0; inputIndex < numInputs; ++inputIndex)
			{
				FString name;
				status = RPR::RPRMaterial::GetNodeInputName(MaterialNode, inputIndex, name);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sCannot get node input name"), *indentation);
					RPR::Error::LogLastError(Context);
					return (status);
				}

				RPR::EMaterialNodeInputType inputType;
				status = RPR::RPRMaterial::GetNodeInputType(MaterialNode, inputIndex, inputType);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%s%s -> Cannot get node input type"), *indentation, *name);
					RPR::Error::LogLastError(Context);
					return (status);
				}

				switch (inputType)
				{
					case EMaterialNodeInputType::Float4:
					status = DumpInputFloat4(name, indentation, MaterialNode, inputIndex);
					break;

					case EMaterialNodeInputType::Image:
					status = DumpInputImage(name, indentation, MaterialNode, inputIndex);
					break;

					case EMaterialNodeInputType::UInt:
					status = DumpInputUint(name, indentation, MaterialNode, inputIndex);
					break;

					case EMaterialNodeInputType::Node:
					{
						RPR::FMaterialNode childNode = nullptr;
						status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, inputIndex, childNode);
						if (RPR::IsResultFailed(status))
						{
							UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Node : Cannot get value"), *indentation, *name);
							return (status);
						}
						UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Node : [%p]"), *indentation, *name, childNode);
						status = InternalDumpMaterialNode(Context, childNode, Depth + 1, RPRXContext);
					}
					break;

					default:
					UE_LOG(LogRPRMaterialNodeDump, Log, TEXT("%sInput '%s' -> Unknown type"), *indentation, *name);
					break;
				}
			}

			return status;
		}

		RPR::FResult DumpMaterialNode(RPR::FContext Context, RPR::FMaterialNode MaterialNode, RPRX::FContext RPRXContext)
		{
			return InternalDumpMaterialNode(Context, MaterialNode, 0, RPRXContext);
		}

	} // namespace RPRMaterial

} // namespace RPR