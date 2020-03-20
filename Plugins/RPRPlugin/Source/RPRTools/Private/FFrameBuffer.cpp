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

#include "FFrameBuffer.h"
#include <Logging/LogMacros.h>

#ifdef CHECK_ERROR
#undef CHECK_ERROR
#endif
#ifdef CHECK_WARNING
#undef CHECK_WARNING
#endif

DEFINE_LOG_CATEGORY_STATIC(LogFFrameBufferFilter, Log, All);

#ifdef CHECK_ERROR
#undef CHECK_ERROR
#endif
#ifdef CHECK_WARNING
#undef CHECK_WARNING
#endif

#define CHECK_ERROR(status, msg) \
	CA_CONSTANT_IF(status != RPR_SUCCESS) { \
		UE_LOG(LogFFrameBufferFilter, Error, msg); \
		return status; \
	}

#define CHECK_WARNING(status, msg) \
	CA_CONSTANT_IF(status != RPR_SUCCESS) { \
		UE_LOG(LogFFrameBufferFilter, Warning, msg); \
	}

namespace RPR
{

FFrameBuffer::FFrameBuffer()
: FrameBuffer(nullptr)
{
}

FFrameBuffer::~FFrameBuffer()
{
	int status;

	status = Destroy();
	CHECK_WARNING(status, TEXT("FFramebuffer destructor failure"));
}

int FFrameBuffer::Create(rpr_context context, rpr_framebuffer_format const format, rpr_framebuffer_desc const* fb_desc)
{
	int status;

	if (!context) {
		CHECK_ERROR(RPR_ERROR_INVALID_CONTEXT, TEXT("nullptr context"));
	}

	status = Destroy();
	CHECK_WARNING(status, TEXT("destroy has some problem"));

	status = rprContextCreateFrameBuffer(context, format, fb_desc, &FrameBuffer);
	CHECK_ERROR(status, TEXT("can't create framebuffer"));

	return RPR_SUCCESS;
}

int FFrameBuffer::Destroy()
{
	if (!FrameBuffer)
		return RPR_SUCCESS;

	int status;
	status = rprObjectDelete(FrameBuffer);
	CHECK_WARNING(status, TEXT("framebuffer object delete error"));

	FrameBuffer = nullptr;

	return status;
}

}
