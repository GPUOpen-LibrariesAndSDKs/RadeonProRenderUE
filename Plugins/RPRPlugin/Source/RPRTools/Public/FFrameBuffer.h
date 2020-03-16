#pragma once

#include <RadeonProRender.h>

namespace RPR
{

class RPRTOOLS_API FFrameBuffer
{
public:
	FFrameBuffer();
	~FFrameBuffer();

	int create(rpr_context context, rpr_framebuffer_format const format, rpr_framebuffer_desc const* fb_desc);
	int destroy();

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
	return out_fb->create(context, format, fb_desc);
}

inline int DestroyFrameBuffer(FFrameBuffer* buffer)
{
	return buffer->destroy();
}

} // namespace RPR
