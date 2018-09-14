#pragma once

namespace RPR
{
	namespace Constants
	{
		namespace MaterialNode
		{
			static const TCHAR* Color0 = TEXT("color0");
			static const TCHAR* Color1 = TEXT("color1");

			namespace Arithmetic
			{
				static const TCHAR* Operation = TEXT("op");
			}

			namespace Lookup
			{
				static const TCHAR* Value = TEXT("value");
			}

			namespace ImageTexture
			{
				static const TCHAR* UV = TEXT("uv");
				static const TCHAR* UV2 = TEXT("uv2");
			}

			namespace UV
			{
				static const TCHAR* XAxis = TEXT("xaxis");
				static const TCHAR* ZAxis = TEXT("zaxis");
				static const TCHAR* UVScale = TEXT("uv_scale");

				namespace Procedural
				{
					static const TCHAR* Origin = TEXT("origin");
					static const TCHAR* Threshold = TEXT("threshold");
					static const TCHAR* UVType = TEXT("uv_type");
				}

				namespace Triplanar
				{
					static const TCHAR* Weight = TEXT("weight");
					static const TCHAR* Offset = TEXT("offset");
				}
			}

		}
	}
}