#include "ImageFilter/ImageFilter.h"

#include "RadeonProRender_CL.h"
#include "RadeonImageFilters_cl.h"
#include "RadeonImageFilters_gl.h"

#include "Async/Async.h"
#include "Async/Future.h"
#include "Async/AsyncWork.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogImageFilter, Log, All);

#define CHECK_ERROR(status, msg) \
	CA_CONSTANT_IF(status != RIF_SUCCESS) { \
		UE_LOG(LogImageFilter, Error, msg); \
		return status; \
	}

#define LOG_ERROR(status, msg)  \
	CA_CONSTANT_IF(status != RIF_SUCCESS) { \
		UE_LOG(LogImageFilter, Error, msg); \
	}

static bool HasGpuContext(rpr_creation_flags contextFlags)
{
#define GPU(x) RPR_CREATION_FLAGS_ENABLE_GPU##x

	rpr_creation_flags gpuMask = GPU(0) | GPU(1) | GPU(2) | GPU(3) | GPU(4) | GPU(5) | GPU(6) | GPU(7) |
		GPU(8) | GPU(9) | GPU(10) | GPU(11) | GPU(12) | GPU(13) | GPU(14) | GPU(15);

#undef GPU

	bool hasGpuContext = (contextFlags & gpuMask) != 0;

	return hasGpuContext;
}

static rpr_int GpuDeviceIdUsed(rpr_creation_flags contextFlags)
{
#define GPU(x) RPR_CREATION_FLAGS_ENABLE_GPU##x

	TArray<rpr_int> gpu_ids =
	{
		GPU(0), GPU(1), GPU(2), GPU(3), GPU(4), GPU(5), GPU(6), GPU(7), GPU(8), GPU(9),
		GPU(10), GPU(11), GPU(12), GPU(13), GPU(14), GPU(15)
	};

#undef GPU

	for (rpr_int i = 0; i < gpu_ids.Num(); i++)
	{
		if ((contextFlags & gpu_ids[i]) != 0)
			return i;
	}

	return -1;
}

int ImageFilter::Initialize(const rpr_context rprContext, uint32 width, uint32 height, const FString& modelsPath)
{
	mWidth = width;
	mHeight = height;
	mModelsPath = modelsPath;

	rpr_creation_flags contextFlags = 0;
	rpr_int rprStatus = rprContextGetInfo(rprContext, RPR_CONTEXT_CREATION_FLAGS, sizeof(rpr_creation_flags), &contextFlags, nullptr);
	CHECK_ERROR(rprStatus, TEXT("Can't get context create flags"));

	if (contextFlags & RPR_CREATION_FLAGS_ENABLE_METAL)
	{
		mRifContext = MakeUnique<RifContextGPUMetal>();
	}
	else if (HasGpuContext(contextFlags))
	{
		mRifContext = MakeUnique<RifContextGPU>();
	}
	else
	{
		mRifContext = MakeUnique<RifContextCPU>();
	}

	rprStatus = mRifContext->Initialize(rprContext);
	CHECK_ERROR(rprStatus, TEXT("Can't initalize image filter context"));

	rif_image_desc desc = {mWidth, mHeight, 0, 0, 0, 4, RIF_COMPONENT_TYPE_FLOAT32};

	rprStatus = mRifContext->CreateOutput(desc);
	CHECK_ERROR(rprStatus, TEXT("Can't create image filter output"));

	return RPR_SUCCESS;
}

ImageFilter::~ImageFilter()
{
	if (mRifFilter)
		mRifFilter->DetachFilter(mRifContext.Get());
}

int ImageFilter::CreateFilter(RifFilterType rifFilteType, bool useOpenImageDenoise)
{
	int status;

	switch (rifFilteType)
	{
	case RifFilterType::BilateralDenoise:
		status = ConstructFilter<RifFilterBilateral>(mRifContext.Get());
		CHECK_ERROR(status, TEXT("can't construct bilateral filter"));
		break;

	case RifFilterType::LwrDenoise:
		status = ConstructFilter<RifFilterLwr>(mRifContext.Get(), mWidth, mHeight);
		CHECK_ERROR(status, TEXT("can't construct lwr filter"));
		break;

	case RifFilterType::EawDenoise:
		status = ConstructFilter<RifFilterEaw>(mRifContext.Get(), mWidth, mHeight);
		CHECK_ERROR(status, TEXT("can't construct eaw filter"));
		break;

	case RifFilterType::MlDenoise:
		status = ConstructFilter<RifFilterMl>(mRifContext.Get(), mWidth, mHeight, mModelsPath, useOpenImageDenoise);
		CHECK_ERROR(status, TEXT("can't construct ml filter"));
		break;

	case RifFilterType::MlDenoiseColorOnly:
		status = ConstructFilter<RifFilterMlColorOnly>(mRifContext.Get(), mWidth, mHeight, mModelsPath, useOpenImageDenoise);
		CHECK_ERROR(status, TEXT("can't construct mlcoloronly filter"));
		break;

	default:
		return RIF_ERROR_INVALID_FILTER;
	}

	return RIF_SUCCESS;
}

int ImageFilter::DeleteFilter()
{
	int status;

	if (!mRifFilter)
		return RIF_SUCCESS;

	status = mRifFilter->DetachFilter(mRifContext.Get());
	CHECK_ERROR(status, TEXT("detaching filter failed"));

	return RIF_SUCCESS;
}

int ImageFilter::AddInput(RifFilterInput inputId, const rpr_framebuffer rprFrameBuffer, float sigma)
{
	rif_image_desc desc = {mWidth, mHeight, 0, 0, 0, 4, RIF_COMPONENT_TYPE_FLOAT32};
	int status;

	rif_image rifImage;
	status = mRifContext->CreateRifImage(rprFrameBuffer, desc, &rifImage);
	CHECK_ERROR(status, TEXT("Can't create rif image for input"));

	if (mRifContext->ContextType() == RifContextType::RifContextGPU || mRifContext->ContextType() == RifContextType::RifContextGPUMetal)
	{
		status = mRifFilter->AddInput(inputId, rifImage, sigma);
		CHECK_ERROR(status, TEXT("Can't add input for rif image (sigma)"));
	}
	else
	{
		status = mRifFilter->AddInput(inputId, rifImage, rprFrameBuffer, sigma);
		CHECK_ERROR(status, TEXT("Can't add input for frame buffer (sigma)"));
	}

	return RIF_SUCCESS;
}

int ImageFilter::AddInput(RifFilterInput inputId, float* memPtr, size_t size, float sigma)
{
	rif_image_desc desc = {mWidth, mHeight, 0, 0, 0, 4, RIF_COMPONENT_TYPE_FLOAT32};

	rif_image rifImage = nullptr;

	rif_int rifStatus = rifContextCreateImage(mRifContext->Context(), &desc, nullptr, &rifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF image"));

	rifStatus = mRifFilter->AddInput(inputId, rifImage, memPtr, size, sigma);
	CHECK_ERROR(rifStatus, TEXT("can't add input to filter"));

	return RIF_SUCCESS;
}

int ImageFilter::AddParam(FString name, RifParam param)
{
	int status;

	status = mRifFilter->AddParam(name, param);
	CHECK_ERROR(status, TEXT("Can't add parameter"));

	return RIF_SUCCESS;
}

int ImageFilter::AttachFilter()
{
	int status;

	status = mRifFilter->AttachFilter(mRifContext.Get());
	CHECK_ERROR(status, TEXT("can't attach filter"));

	status = mRifFilter->ApplyParameters();
	CHECK_ERROR(status, TEXT("can't apply filter parameters"));

	return RIF_SUCCESS;
}

int ImageFilter::Run()
{
	rif_int rifStatus;

	rifStatus = mRifContext->UpdateInputs(mRifFilter.Get());
	CHECK_ERROR(rifStatus, TEXT("can't update inputs"));

	rifStatus = rifContextExecuteCommandQueue(mRifContext->Context(), mRifContext->Queue(), nullptr, nullptr, nullptr);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to execute queue"));

	return RIF_SUCCESS;
}

int ImageFilter::GetData(TArray<float>* outData)
{
	float* output = nullptr;

	rif_int rifStatus = rifImageMap(mRifContext->Output(), RIF_IMAGE_MAP_READ, (void**)&output);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to map output data"));

	if (!output)
		CHECK_ERROR(RIF_ERROR_INVALID_IMAGE, TEXT("RPR denoiser failed to map output data"));

	TArray<float> floatData(output, mWidth * mHeight * 4);

	rifStatus = rifImageUnmap(mRifContext->Output(), output);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser: can't unmap image"));

	*outData = std::move(floatData);

	return RIF_SUCCESS;
}

RifContextWrapper::~RifContextWrapper()
{
	rif_int rifStatus = RIF_SUCCESS;

	if (mOutputRifImage != nullptr)
	{
		rifStatus = rifObjectDelete(mOutputRifImage);
		LOG_ERROR(rifStatus, TEXT("Can't delete rif object mOutputRifImage"));
	}

	if (mRifCommandQueueHandle != nullptr)
	{
		rifStatus = rifObjectDelete(mRifCommandQueueHandle);
		LOG_ERROR(rifStatus, TEXT("Can't delete rif object mRifCommandQueueHandle"));
	}

	if (mRifContextHandle != nullptr)
	{
		rifStatus = rifObjectDelete(mRifContextHandle);
		LOG_ERROR(rifStatus, TEXT("Can't delete rif object mRfiContextHandle"));
	}
}

const rif_context RifContextWrapper::Context() const
{
	return mRifContextHandle;
}

const rif_command_queue RifContextWrapper::Queue() const
{
	return mRifCommandQueueHandle;
}

const rif_image RifContextWrapper::Output() const
{
	return mOutputRifImage;
}

const RifContextType RifContextWrapper::ContextType() const
{
	return mContextType;
}

int RifContextWrapper::CreateOutput(const rif_image_desc& desc)
{
	rif_int rifStatus = rifContextCreateImage(mRifContextHandle, &desc, nullptr, &mOutputRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create output image"));

	return RIF_SUCCESS;
}

int RifContextWrapper::GetRprCachePath(rpr_context rprContext, TArray<rpr_char>* path)
{
	size_t length;
	rpr_status rprStatus = rprContextGetInfo(rprContext, RPR_CONTEXT_CACHE_PATH, sizeof(size_t), nullptr, &length);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser: failed to get context cache path"));

	path->SetNum(length);
	rpr_char* pathPtr = &path->operator[](0);
	rprStatus = rprContextGetInfo(rprContext, RPR_CONTEXT_CACHE_PATH, path->GetAllocatedSize() / sizeof(rpr_char), pathPtr, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get cache path"));

	return RPR_SUCCESS;
}

int RifContextWrapper::UpdateInputs(const RifFilterWrapper* rifFilter)
{
	int status = 0;
	for (const auto& input : rifFilter->mInputs)
	{
		// note: accamulate errors.
		status |= input.Value->Update();
	}

	return status;
}

int RifContextGPU::Initialize(rpr_context rprContext)
{
#if defined(_WIN32) || defined(__linux__)
	mContextType = RifContextType::RifContextGPU;

	int deviceCount = 0;

	rif_int rifStatus = rifGetDeviceCount(rifBackendApiType, &deviceCount);
	CHECK_ERROR(rifStatus, TEXT("Can't get denoiser compatible devices"));

	if (0 == deviceCount) {
		CHECK_ERROR(RIF_ERROR_COMPUTE_API_NOT_SUPPORTED, TEXT("No available denoiser devices"));
	}

	rpr_cl_context clContext;
	rpr_int rprStatus = rprContextGetInfo(rprContext, RPR_CL_CONTEXT, sizeof(rpr_cl_context), &clContext, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get CL device context."));

	rpr_cl_device clDevice;
	rprStatus = rprContextGetInfo(rprContext, RPR_CL_DEVICE, sizeof(rpr_cl_device), &clDevice, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get CL device."));

	rpr_cl_command_queue clCommandQueue;
	rprStatus = rprContextGetInfo(rprContext, RPR_CL_COMMAND_QUEUE, sizeof(rpr_cl_command_queue), &clCommandQueue, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get CL command queue."));

	TArray<rpr_char> path;
	rprStatus = GetRprCachePath(rprContext, &path);
	CHECK_ERROR(rifStatus, TEXT("Can't get cache path, abort rif context gpu initalization"));

	rifStatus = rifCreateContextFromOpenClContext(RIF_API_VERSION, clContext, clDevice, clCommandQueue, path.GetData(), &mRifContextHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF context."));

	rifStatus = rifContextCreateCommandQueue(mRifContextHandle, &mRifCommandQueueHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF command queue."));
#else
	CHECK_ERROR(RIF_ERROR_COMPUTE_API_NOT_SUPPORTED, TEXT("RifContextGPU is not implemented on OSX"));
#endif

	return RIF_SUCCESS;
}

int RifContextGPU::CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* rifImage)
{
	if (!rifImage)
		return RIF_ERROR_INVALID_IMAGE;

#if defined(_WIN32) || defined(__linux__)
	rpr_cl_mem clMem = nullptr;

	rpr_int rprStatus = rprFrameBufferGetInfo(rprFrameBuffer, RPR_CL_MEM_OBJECT, sizeof(rpr_cl_mem), &clMem, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get frame buffer info"));

	rif_int rifStatus = rifContextCreateImageFromOpenClMemory(mRifContextHandle, &desc, clMem, false, rifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to get image from opencl memory"));
#else
	CHECK_ERROR(RIF_ERROR_INTERNAL_ERROR, TEXT("RPR denoiser: CreateRifImage not supported"));
#endif
	return RIF_SUCCESS;
}

int RifContextCPU::Initialize(rpr_context rprContext)
{
	mContextType = RifContextType::RifContextCPU;
	int deviceCount = 0;
	rif_int rifStatus = rifGetDeviceCount(rifBackendApiType, &deviceCount);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser hasn't found compatible devices."));
	if (deviceCount <= 0) {
		CHECK_ERROR(RIF_ERROR_IO_ERROR, TEXT("RPR denoiser hasn't found compatible devices."));
	}

	TArray<rpr_char> path;
	rifStatus = GetRprCachePath(rprContext, &path);
	CHECK_ERROR(rifStatus, TEXT("can't get cache path"));

	rifStatus = rifCreateContext(RIF_API_VERSION, rifBackendApiType, 0, path.GetData(), &mRifContextHandle);
	CHECK_ERROR(rifStatus, TEXT("can't create CPU context"));

	rifStatus = rifContextCreateCommandQueue(mRifContextHandle, &mRifCommandQueueHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF command queue"));

	return RIF_SUCCESS;
}

int RifContextCPU::CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* rifImage)
{
	rif_int rifStatus = rifContextCreateImage(mRifContextHandle, &desc, nullptr, rifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF image"));

	return RIF_SUCCESS;
}

int RifContextGPUMetal::Initialize(rpr_context rprContext)
{
	mContextType = RifContextType::RifContextGPUMetal;
	int deviceCount = 0;
	rif_int rifStatus;

	rifStatus = rifGetDeviceCount(rifBackendApiType, &deviceCount);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser hasn't found compatible devices"));

	if (deviceCount <= 0) {
		CHECK_ERROR(RPR_ERROR_IO_ERROR, TEXT("0 available devices to run GPU metal"))
	}

	rpr_creation_flags contextFlags = 0;
	rpr_int rprStatus = rprContextGetInfo(rprContext, RPR_CONTEXT_CREATION_FLAGS, sizeof(rpr_creation_flags), &contextFlags, nullptr);
	CHECK_ERROR(rprStatus, TEXT("Can't get context creation flags"));

	TArray<rpr_char> path;
	rifStatus = GetRprCachePath(rprContext, &path);
	CHECK_ERROR(rifStatus, TEXT("can't get rpr cache path"));

	// we find the active gpu from the rpr contextFlags and then use that to create the rif context
	rifStatus = rifCreateContext(RIF_API_VERSION, rifBackendApiType, GpuDeviceIdUsed(contextFlags), path.GetData(), &mRifContextHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF context"));

	rifStatus = rifContextCreateCommandQueue(mRifContextHandle, &mRifCommandQueueHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create RIF command queue"));

	return RIF_SUCCESS;
}

int RifContextGPUMetal::CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* rifImage)
{
#if defined(__APPLE__)
	rpr_cl_mem clMem = nullptr;

	rpr_int rprStatus = rprFrameBufferGetInfo(rprFrameBuffer, RPR_CL_MEM_OBJECT, sizeof(rpr_cl_mem), &clMem, nullptr);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to get frame buffer info"));

	size_t fbSize;
	rprStatus = rprFrameBufferGetInfo(rprFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, NULL, &fbSize);

	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to acquire frame buffer info"));

	rif_int rifStatus = rifContextCreateImageFromMetalMemory(mRifContextHandle, &desc, clMem, fbSize, rifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to get frame buffer info"));

	return RIF_SUCCESS;
#endif
	return RIF_ERROR_UNSUPPORTED;
}

RifInput::RifInput(rif_image rifImage, float sigma) :
	mRifImage(rifImage),
	mSigma(sigma)
{
}

RifInput::~RifInput()
{
	rifObjectDelete(mRifImage);
}

RifInputGPU::RifInputGPU(rif_image rifImage, float sigma) :
	RifInput(rifImage, sigma)
{
}

RifInputGPUCPU::RifInputGPUCPU(rif_image rifImage, const rpr_framebuffer rprFrameBuffer, float sigma) :
	RifInput(rifImage, sigma),
	mRprFrameBuffer(rprFrameBuffer)
{
}

int RifInputGPUCPU::Update()
{
	size_t sizeInBytes = 0;
	size_t retSize = 0;
	void* imageData = nullptr;

	// verify image size
	rif_int rifStatus = rifImageGetInfo(mRifImage, RIF_IMAGE_DATA_SIZEBYTE, sizeof(size_t), (void*)&sizeInBytes, &retSize);
	CHECK_ERROR(rifStatus, TEXT("ImageFilter: input gpu+cpu. Can't get image data size"));

	size_t fbSize;
	rpr_int rprStatus = rprFrameBufferGetInfo(mRprFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, NULL, &fbSize);
	CHECK_ERROR(rprStatus, TEXT("RPR denoiser failed to acquire frame buffer info"));

	if (sizeInBytes != fbSize)
		CHECK_ERROR(RIF_ERROR_INTERNAL_ERROR, TEXT("RPR denoiser failed to match RIF image and frame buffer sizes"));

	// resolve framebuffer data to rif image
	rifStatus = rifImageMap(mRifImage, RIF_IMAGE_MAP_WRITE, &imageData);
	CHECK_ERROR(rifStatus, TEXT("RPR denoise failed to acquire RIF image"));

	if (!imageData)
		CHECK_ERROR(RIF_ERROR_INTERNAL_ERROR, TEXT("RPR denoise failed to acquire RIF image"));

	rprStatus = rprFrameBufferGetInfo(mRprFrameBuffer, RPR_FRAMEBUFFER_DATA, fbSize, imageData, NULL);
	CHECK_ERROR(rprStatus, TEXT("can't get framebuffer data"));

	// try to unmap at first, then raise a possible error
	rifStatus = rifImageUnmap(mRifImage, imageData);
	CHECK_ERROR(rifStatus, TEXT("failed to unmap output data"));

	return RIF_SUCCESS;
}

RifInputCPU::RifInputCPU(rif_image rifImage, float* memPtr, size_t size, float sigma) :
	RifInput(rifImage, sigma),
	mMemPtr(memPtr),
	mSize(size)
{
}

int RifInputCPU::Update()
{
	size_t sizeInBytes = 0;
	size_t retSize = 0;
	void* imageData = nullptr;

	// verify image size
	rif_int rifStatus = rifImageGetInfo(mRifImage, RIF_IMAGE_DATA_SIZEBYTE, sizeof(size_t), (void*)&sizeInBytes, &retSize);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to get RIF image info"));

	if (sizeInBytes != mSize)
		CHECK_ERROR(RIF_ERROR_INTERNAL_ERROR, TEXT("sizeInBytes != mSize. Buffer internal check error"));

	// copy data to rif image
	rifStatus = rifImageMap(mRifImage, RIF_IMAGE_MAP_WRITE, &imageData);
	CHECK_ERROR(rifStatus, TEXT("Can't get rif image map with write access"));

	FMemory::Memcpy(imageData, mMemPtr, mSize);

	rifStatus = rifImageUnmap(mRifImage, imageData);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to unmap output data"));

	return RIF_SUCCESS;
}

RifFilterWrapper::~RifFilterWrapper()
{
	mInputs.Empty();

	for (const rif_image& auxImage : mAuxImages)
	{
		rifObjectDelete(auxImage);
	}

	for (const rif_image_filter& auxFilter : mAuxFilters)
	{
		rifObjectDelete(auxFilter);
	}

	if (mRifImageFilterHandle != nullptr)
	{
		rifObjectDelete(mRifImageFilterHandle);
	}
}

int RifFilterWrapper::AddInput(RifFilterInput inputId, const rif_image rifImage, float sigma)
{
	mInputs.Add(inputId, MakeShared<RifInputGPU>(rifImage, sigma));
	return RIF_SUCCESS;
}

int RifFilterWrapper::AddInput(RifFilterInput inputId, const rif_image rifImage, const rpr_framebuffer rprFrameBuffer, float sigma)
{
	mInputs.Add(inputId, MakeShared<RifInputGPUCPU>(rifImage, rprFrameBuffer, sigma));
	return RIF_SUCCESS;
}

int RifFilterWrapper::AddInput(RifFilterInput inputId, const rif_image rifImage, float* memPtr, size_t size, float sigma)
{
	mInputs.Add(inputId, MakeShared<RifInputCPU>(rifImage, memPtr, size, sigma));
	return RIF_SUCCESS;
}

int RifFilterWrapper::AddParam(FString name, RifParam param)
{
	mParams.Add(name, param);
	return RIF_SUCCESS;
}

int RifFilterWrapper::DetachFilter(const RifContextWrapper* rifContext)
{
	rif_int rifStatus = RIF_SUCCESS;

	for (const rif_image_filter& auxFilter : mAuxFilters)
	{
		rifStatus = rifCommandQueueDetachImageFilter(rifContext->Queue(), auxFilter);
		CHECK_ERROR(rifStatus, TEXT("can't detach command queue auxFilter"));
	}

	rifStatus = rifCommandQueueDetachImageFilter(rifContext->Queue(), mRifImageFilterHandle);
	CHECK_ERROR(rifStatus, TEXT("can't detach command queue image filter handle"))

	return RIF_SUCCESS;
}

int RifFilterWrapper::SetupVarianceImageFilter(const rif_image_filter inputFilter, const rif_image outVarianceImage)
{
	rif_int rifStatus = rifImageFilterSetParameterImage(inputFilter, "positionsImg", mInputs.FindRef(RifWorldCoordinate)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set parameter positionsImg"));

	rifStatus = rifImageFilterSetParameterImage(inputFilter, "normalsImg", mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set normalsImg"));

	rifStatus = rifImageFilterSetParameterImage(inputFilter, "meshIdsImg", mInputs.FindRef(RifObjectId)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set meshIdsImg"));

	rifStatus = rifImageFilterSetParameterImage(inputFilter, "outVarianceImg", outVarianceImage);
	CHECK_ERROR(rifStatus, TEXT("can't set outVarianceImg"));

	return RIF_SUCCESS;
}

int RifFilterWrapper::ApplyParameters()
{
	rif_int rifStatus;

	for (const auto& param : mParams)
	{
		switch (param.Value.mType)
		{
		case RifParamType::RifInt:
			rifStatus = rifImageFilterSetParameter1u(mRifImageFilterHandle, TCHAR_TO_ANSI(*param.Key), param.Value.mData.i);
			CHECK_ERROR(rifStatus, TEXT("can't setup 1u parameter"));
			break;

		case RifParamType::RifFloat:
			rifStatus = rifImageFilterSetParameter1f(mRifImageFilterHandle, TCHAR_TO_ANSI(*param.Key), param.Value.mData.f);
			CHECK_ERROR(rifStatus, TEXT("can't setup 1u parameter"));
			break;
		}
	}

	return RIF_SUCCESS;
}

int RifFilterBilateral::Initialize(const RifContextWrapper* rifContext)
{
	rif_int rifStatus;

	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_BILATERAL_DENOISE, &mRifImageFilterHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create Bilateral filter"));

	return RIF_SUCCESS;
}

int RifFilterBilateral::AttachFilter(const RifContextWrapper* rifContext)
{
	for (const auto& input : mInputs)
	{
		inputImages.Push(input.Value->mRifImage);
		sigmas.Push(input.Value->mSigma);
	}

	rif_int rifStatus = rifImageFilterSetParameterImageArray(mRifImageFilterHandle, "inputs", &inputImages[0], static_cast<rif_int>(inputImages.Num()));
	CHECK_ERROR(rifStatus, TEXT("can't set parameter input for image arrray"));

	rifStatus = rifImageFilterSetParameterFloatArray(mRifImageFilterHandle, "sigmas", &sigmas[0], static_cast<rif_int>(sigmas.Num()));
	CHECK_ERROR(rifStatus, TEXT("can't set sigmas"));

	rifStatus = rifImageFilterSetParameter1u(mRifImageFilterHandle, "inputsNum", static_cast<rif_int>(inputImages.Num()));
	CHECK_ERROR(rifStatus, TEXT("can't set inputsNum"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mRifImageFilterHandle, mInputs.FindRef(RifColor)->mRifImage, rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	return RIF_SUCCESS;
}

int RifFilterLwr::Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height)
{
	// main LWR filter
	rif_int rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_LWR_DENOISE, &mRifImageFilterHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create LWR filter"));

	// auxillary LWR filters
	mAuxFilters.SetNumZeroed(AuxFilterMax);

	for (rif_image_filter& auxFilter : mAuxFilters)
	{
		rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_TEMPORAL_ACCUMULATOR, &auxFilter);
		CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary filter"));
	}

	// auxillary LWR images
	rif_image_desc desc = {width, height, 0, 0, 0, 4, RIF_COMPONENT_TYPE_FLOAT32};

	mAuxImages.SetNumZeroed(AuxImageMax);

	for (rif_image& auxImage : mAuxImages)
	{
		rifStatus = rifContextCreateImage(rifContext->Context(), &desc, nullptr, &auxImage);
		CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary image"));
	}

	return RIF_SUCCESS;
}

int RifFilterLwr::AttachFilter(const RifContextWrapper* rifContext)
{
	rif_int rifStatus;

	// make variance image filters
	rifStatus = SetupVarianceImageFilter(mAuxFilters[ColorVar], mAuxImages[ColorVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't setup variance color image filter"));

	rifStatus = SetupVarianceImageFilter(mAuxFilters[NormalVar], mAuxImages[NormalVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't setup variance normal image filter"));

	rifStatus = SetupVarianceImageFilter(mAuxFilters[DepthVar], mAuxImages[DepthVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't setup variance depth image filter"));

	rifStatus = SetupVarianceImageFilter(mAuxFilters[TransVar], mAuxImages[TransVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't setup trans depth image filter"));

	// Configure Filter
	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "vColorImg", mAuxImages[ColorVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't set vColorImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "normalsImg", mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set normalsImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "vNormalsImg", mAuxImages[NormalVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't set vNormalsImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "depthImg", mInputs.FindRef(RifDepth)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set depthImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "vDepthImg", mAuxImages[DepthVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't set vDepthImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "transImg", mInputs.FindRef(RifTrans)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set transImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "vTransImg", mAuxImages[TransVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't set vTransImg"));

	// attach filters
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[TransVar],
		mInputs.FindRef(RifTrans)->mRifImage, mAuxImages[TransVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't attach image filter trans"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[DepthVar],
			mInputs.FindRef(RifDepth)->mRifImage, mAuxImages[DepthVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't attach image filter depth"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[NormalVar],
			mInputs.FindRef(RifNormal)->mRifImage, mAuxImages[NormalVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't attach image filter normal"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[ColorVar],
			mInputs.FindRef(RifColor)->mRifImage, mAuxImages[ColorVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't attach image filter color"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mRifImageFilterHandle,
			mInputs.FindRef(RifColor)->mRifImage, rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("can't attach image filter"));

	return RIF_SUCCESS;
}

int RifFilterEaw::Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height)
{
	// main EAW filter
	rif_int rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_EAW_DENOISE, &mRifImageFilterHandle);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create EAW filter"));

	// auxillary EAW filters
	mAuxFilters.SetNumZeroed(AuxFilterMax);

	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_TEMPORAL_ACCUMULATOR, &mAuxFilters[ColorVar]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary filter"));

	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_MLAA, &mAuxFilters[Mlaa]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary filter"));

	// auxillary rif images
	rif_image_desc desc = {width, height, 0, 0, 0, 4, RIF_COMPONENT_TYPE_FLOAT32};

	mAuxImages.SetNumZeroed(AuxImageMax);

	rifStatus = rifContextCreateImage(rifContext->Context(), &desc, nullptr, &mAuxImages[ColorVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary image"));

	rifStatus = rifContextCreateImage(rifContext->Context(), &desc, nullptr, &mAuxImages[DenoisedOutputImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create auxillary image."));

	return RIF_SUCCESS;
}

int RifFilterEaw::AttachFilter(const RifContextWrapper* rifContext)
{
	// setup inputs
	rif_int rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "normalsImg", mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("Can't set input normalsImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "transImg", mInputs.FindRef(RifTrans)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("Can't set input transImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "colorVar", mInputs.FindRef(RifColor)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("Can't set input colorVar"));

	// setup sigmas
	rifStatus = rifImageFilterSetParameter1f(mRifImageFilterHandle, "colorSigma", mInputs.FindRef(RifColor)->mSigma);
	CHECK_ERROR(rifStatus, TEXT("Can't set input colorSigma"));

	rifStatus = rifImageFilterSetParameter1f(mRifImageFilterHandle, "normalSigma", mInputs.FindRef(RifNormal)->mSigma);
	CHECK_ERROR(rifStatus, TEXT("Can't set input normalSigma"));

	rifStatus = rifImageFilterSetParameter1f(mRifImageFilterHandle, "depthSigma", mInputs.FindRef(RifDepth)->mSigma);
	CHECK_ERROR(rifStatus, TEXT("Can't set input depthSigma"));

	rifStatus = rifImageFilterSetParameter1f(mRifImageFilterHandle, "transSigma", mInputs.FindRef(RifTrans)->mSigma);
	CHECK_ERROR(rifStatus, TEXT("Can't set input transSigma"));

	// setup color variance filter
	rifStatus = SetupVarianceImageFilter(mAuxFilters[ColorVar], mAuxImages[ColorVarianceImage]);
	CHECK_ERROR(rifStatus, TEXT("can't setup variance color filter"));

	// setup MLAA filter
	rifStatus = rifImageFilterSetParameterImage(mAuxFilters[Mlaa], "normalsImg", mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to apply parameters"));

	rifStatus = rifImageFilterSetParameterImage(mAuxFilters[Mlaa], "meshIDImg", mInputs.FindRef(RifObjectId)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to apply parameters"));

	// attach filters
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[ColorVar],
		mInputs.FindRef(RifColor)->mRifImage, rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mRifImageFilterHandle, rifContext->Output(),
		mAuxImages[DenoisedOutputImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[Mlaa], mAuxImages[DenoisedOutputImage],
		rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	return RIF_SUCCESS;
}

int RifFilterMl::Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height, const FString& modelsPath, bool useOpenImageDenoise)
{
	rif_int rifStatus;

	// main ML filter
	if (useOpenImageDenoise)
	{
		rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_OPENIMAGE_DENOISE, &mRifImageFilterHandle);
		CHECK_ERROR(rifStatus, TEXT("can't create ML filter"))
	}
	else
	{
		rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_AI_DENOISE, &mRifImageFilterHandle);
		CHECK_ERROR(rifStatus, TEXT("can't create ML filter"))
	}

	rifStatus = rifImageFilterSetParameterString(mRifImageFilterHandle, "modelPath", TCHAR_TO_ANSI(*modelsPath));
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set ML filter models path"));

	mAuxImages.SetNumZeroed(AuxImageMax);

	// temporary output for ML denoiser with 3 components per pixel (by design of the filter)
	rif_image_desc desc = {width, height, 0, 0, 0, 3, RIF_COMPONENT_TYPE_FLOAT32};

	rifStatus = rifContextCreateImage(rifContext->Context(), &desc, nullptr, &mAuxImages[MlOutputRifImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create output image"));

	// Normals remap filter setup
	mAuxFilters.SetNumZeroed(AuxFilterMax);

	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_REMAP_RANGE, &mAuxFilters[NormalsRemapFilter]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create remap filter"));

	rifStatus = rifImageFilterSetParameter1f(mAuxFilters[NormalsRemapFilter], "dstLo", 0.0f);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set dstLo"));

	rifStatus = rifImageFilterSetParameter1f(mAuxFilters[NormalsRemapFilter], "dstHi", +1.0f);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set remap filter parameters"));

	// Depth remap filter setup
	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_REMAP_RANGE, &mAuxFilters[DepthRemapFilter]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create remap filter"));

	rifStatus = rifImageFilterSetParameter1f(mAuxFilters[DepthRemapFilter], "dstLo", 0.0f);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set dstLo"));

	rifStatus = rifImageFilterSetParameter1f(mAuxFilters[DepthRemapFilter], "dstHi", 1.0f);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set dstHi"));

	// resampler filter setup
	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_RESAMPLE, &mAuxFilters[OutputResampleFilter]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create resampler filter"));

	rifStatus = rifImageFilterSetParameter1u(mAuxFilters[OutputResampleFilter], "interpOperator", RIF_IMAGE_INTERPOLATION_NEAREST);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set interpOperator"));

	rifStatus = rifImageFilterSetParameter2u(mAuxFilters[OutputResampleFilter], "outSize", width, height);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set outSize"));

	return RIF_SUCCESS;
}

int RifFilterMl::AttachFilter(const RifContextWrapper* rifContext)
{
	rif_int rifStatus;

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "colorImg", mInputs.FindRef(RifColor)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set image parameter colorImg"));

	rifStatus = rifImageFilterSetParameter1u(mRifImageFilterHandle, "useHDR", 1);
	CHECK_ERROR(rifStatus, TEXT("can't set image parameter useHDR"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "normalsImg", mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set image parameter normalsImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "depthImg", mInputs.FindRef(RifDepth)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set image parameter deptImg"));

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "albedoImg", mInputs.FindRef(RifAlbedo)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("can't set image parameter abledoImg"));

	// attach remap filters
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[NormalsRemapFilter],
		mInputs.FindRef(RifNormal)->mRifImage, mInputs.FindRef(RifNormal)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[DepthRemapFilter],
		mInputs.FindRef(RifDepth)->mRifImage, mInputs.FindRef(RifDepth)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue"));

	// attach ML filter (main)
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mRifImageFilterHandle,
		mInputs.FindRef(RifColor)->mRifImage, mAuxImages[MlOutputRifImage]);
	CHECK_ERROR(rifStatus, TEXT("can't attach ML filter (main)"));

	// attach output resampler filter
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[OutputResampleFilter],
		mAuxImages[MlOutputRifImage], rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach resampler filter to queue"));

	return RIF_SUCCESS;
}

int RifFilterMlColorOnly::Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height,
	const FString& modelsPath, bool useOpenImageDenoise)
{
	rif_int rifStatus;

	// main ML filter
	if (useOpenImageDenoise)
	{
		rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_OPENIMAGE_DENOISE, &mRifImageFilterHandle);
		CHECK_ERROR(rifStatus, TEXT("can't create image filter OPENIMAGE_DENOISE"));
	}
	else
	{
		rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_AI_DENOISE, &mRifImageFilterHandle);
		CHECK_ERROR(rifStatus, TEXT("can't create image filter AI_DENOISE"));
	}

	rifStatus = rifImageFilterSetParameterString(mRifImageFilterHandle, "modelPath", TCHAR_TO_ANSI(*modelsPath));
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set ML filter models path"));

	mAuxImages.SetNumZeroed(AuxImageMax);

	// temporary output for ML denoiser with 3 components per pixel (by design of the filter)
	rif_image_desc desc = {width, height, 0, 0, 0, 3, RIF_COMPONENT_TYPE_FLOAT32};

	rifStatus = rifContextCreateImage(rifContext->Context(), &desc, nullptr, &mAuxImages[MlOutputRifImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create output image"));

	mAuxFilters.SetNumZeroed(AuxFilterMax);

	// resampler filter setup
	rifStatus = rifContextCreateImageFilter(rifContext->Context(), RIF_IMAGE_FILTER_RESAMPLE, &mAuxFilters[OutputResampleFilter]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to create resampler filter"));

	rifStatus = rifImageFilterSetParameter1u(mAuxFilters[OutputResampleFilter], "interpOperator", RIF_IMAGE_INTERPOLATION_NEAREST);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set interpOperator parameters."));

	rifStatus = rifImageFilterSetParameter2u(mAuxFilters[OutputResampleFilter], "outSize", width, height);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set outSize parameters."));

	return RIF_SUCCESS;
}

int RifFilterMlColorOnly::AttachFilter(const RifContextWrapper* rifContext)
{
	rif_int rifStatus;

	rifStatus = rifImageFilterSetParameterImage(mRifImageFilterHandle, "colorImg", mInputs.FindRef(RifColor)->mRifImage);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set parater colorImg"));

	rifStatus = rifImageFilterSetParameter1u(mRifImageFilterHandle, "useHDR", 1);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to set parater useHDR"));

	// attach ML filter (main)
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mRifImageFilterHandle,
		mInputs.FindRef(RifColor)->mRifImage, mAuxImages[MlOutputRifImage]);
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach filter to queue."));

	// attach output resampler filter
	rifStatus = rifCommandQueueAttachImageFilter(rifContext->Queue(), mAuxFilters[OutputResampleFilter],
		mAuxImages[MlOutputRifImage], rifContext->Output());
	CHECK_ERROR(rifStatus, TEXT("RPR denoiser failed to attach resampler filter to queue"));

	return RIF_SUCCESS;
}
