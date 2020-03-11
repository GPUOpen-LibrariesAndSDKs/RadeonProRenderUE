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

#pragma once
#include "RPRCompatibility.h"
#include "PixelFormat.h"
#include "Engine/Texture.h"
#include "RHIDefinitions.h"

class RPRCOMPATIBILITY_API FRPRCpTexture2DDynamic
{
public:

    struct FCreateInfo
    {
        EPixelFormat Format;
        bool bIsResolveTarget;
        bool bSRGB;
        TextureFilter Filter;
        ESamplerAddressMode SamplerAddressMode;
    };

public:

    static UTexture2DDynamic* Create(int32 SizeX, int32 SizeY, const FCreateInfo& CreateInfo);

};
