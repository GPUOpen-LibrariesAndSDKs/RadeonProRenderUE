#include "FFrameBuffer.h"

namespace RPR
{

FFrameBuffer::FFrameBuffer()
: FrameBuffer(nullptr)
{
}

FFrameBuffer::~FFrameBuffer()
{
	destroy();
}

int FFrameBuffer::create(rpr_context context, rpr_framebuffer_format const format, rpr_framebuffer_desc const* fb_desc)
{
	if (!context) {
		return RPR_ERROR_INVALID_CONTEXT;
	}

	destroy();

	int status;

	status = rprContextCreateFrameBuffer(context, format, fb_desc, &FrameBuffer);
	return status;
}

int FFrameBuffer::destroy()
{
	if (!FrameBuffer)
		return RPR_SUCCESS;

	int status;
	status = rprObjectDelete(FrameBuffer);

	FrameBuffer = nullptr;

	return status;
}

}
