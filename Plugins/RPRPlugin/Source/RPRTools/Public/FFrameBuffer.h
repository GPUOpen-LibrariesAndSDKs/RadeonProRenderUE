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

#include <RadeonProRender.h>

#ifdef RPRTOOLS_API
#define FRAMBUFFER_DLL_API __declspec( dllexport )
#else
#define FRAMBUFFER_DLL_API __declspec( dllimport )
#endif

namespace RPR
{

class FRAMBUFFER_DLL_API FFrameBuffer
{
public:
	FFrameBuffer();
	~FFrameBuffer();

	int Create(rpr_context context, rpr_framebuffer_format const format, rpr_framebuffer_desc const* fb_desc);
	int Destroy();

	operator bool() const { return FrameBuffer != nullptr; }

	operator rpr_framebuffer() { return FrameBuffer; }
	rpr_framebuffer get()      { return FrameBuffer; }

	FFrameBuffer(const FFrameBuffer&)            = delete;
	FFrameBuffer& operator=(const FFrameBuffer&) = delete;
private:
	rpr_framebuffer FrameBuffer;
};

inline int ContextCreateFrameBuffer(rpr_context context, rpr_framebuffer_format const format, rpr_framebuffer_desc const* fb_desc, FFrameBuffer* out_fb)
{
	return out_fb->Create(context, format, fb_desc);
}

inline int DestroyFrameBuffer(FFrameBuffer* buffer)
{
	return buffer->Destroy();
}

} // namespace RPR
