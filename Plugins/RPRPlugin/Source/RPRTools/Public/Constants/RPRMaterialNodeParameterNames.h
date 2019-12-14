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