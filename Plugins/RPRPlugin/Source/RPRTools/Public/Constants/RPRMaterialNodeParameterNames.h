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

namespace RPR
{
	namespace Constants
	{
		namespace MaterialNode
		{
			static const unsigned int Color = RPR_MATERIAL_INPUT_COLOR;
			static const unsigned int Color0 = RPR_MATERIAL_INPUT_COLOR0;
			static const unsigned int Color1 = RPR_MATERIAL_INPUT_COLOR1;
			static const unsigned int BumpScale = RPR_MATERIAL_INPUT_SCALE;

			namespace Arithmetic
			{
				static const unsigned int Operation = RPR_MATERIAL_INPUT_OP;
			}

			namespace Lookup
			{
				static const unsigned int Value = RPR_MATERIAL_INPUT_VALUE;
			}

			namespace ImageTexture
			{
				static const unsigned int ImageData = RPR_MATERIAL_INPUT_DATA;
				static const unsigned int UV = RPR_MATERIAL_INPUT_UV;
				static const unsigned int UV2 = RPR_MATERIAL_INPUT_UV;
			}

			namespace UV
			{
				static const unsigned int XAxis =  RPR_MATERIAL_INPUT_XAXIS;
				static const unsigned int ZAxis = RPR_MATERIAL_INPUT_ZAXIS;
				static const unsigned int UVScale = RPR_MATERIAL_INPUT_UV_SCALE;

				namespace Procedural
				{
					static const unsigned int Origin = RPR_MATERIAL_INPUT_ORIGIN;
					static const unsigned int Threshold = RPR_MATERIAL_INPUT_THRESHOLD;
					static const unsigned int UVType = RPR_MATERIAL_INPUT_UV_TYPE;
				}

				namespace Triplanar
				{
					static const unsigned int Weight = RPR_MATERIAL_INPUT_WEIGHT;
					static const unsigned int Offset = RPR_MATERIAL_INPUT_OFFSET;
				}
			}

		}
	}
}