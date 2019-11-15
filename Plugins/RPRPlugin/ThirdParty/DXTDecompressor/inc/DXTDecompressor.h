// LICENSE
// This file is distributed under the terms and conditions of the MIT
// license. This license is specified at the top of each source fileand must be
// preserved in its entirety.
//

#pragma once

namespace DXTDecompressor
{
	void DecompressImage(unsigned char* output_rgba_raw_data, int texture_width, int texture_height, void const* dxt_blocks_raw_data);
}