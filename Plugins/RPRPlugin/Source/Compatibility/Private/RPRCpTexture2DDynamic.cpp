/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "RPRCpTexture2DDynamic.h"
#include "Engine/Texture2DDynamic.h"

#if ENGINE_MINOR_VERSION < 20

UTexture2DDynamic* FRPRCpTexture2DDynamic::Create(int32 SizeX, int32 SizeY, const FCreateInfo& CreateInfo)
{
    return (UTexture2DDynamic::Create(SizeX, SizeY, CreateInfo.Format, CreateInfo.bIsResolveTarget));
}

#elif ENGINE_MINOR_VERSION >= 20

UTexture2DDynamic* FRPRCpTexture2DDynamic::Create(int32 SizeX, int32 SizeY, const FCreateInfo& CreateInfo)
{
    FTexture2DDynamicCreateInfo nativeCreateInfo(
        CreateInfo.Format,
        CreateInfo.bIsResolveTarget,
        CreateInfo.bSRGB,
        CreateInfo.Filter,
        CreateInfo.SamplerAddressMode
    );
    return (UTexture2DDynamic::Create(SizeX, SizeY, nativeCreateInfo));
}

#endif
