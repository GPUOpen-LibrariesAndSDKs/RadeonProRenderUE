#pragma once

#include "CoreMinimal.h"

#include "RadeonProRender.h"
#define RIF_STATIC_LIBRARY 0
#include "RadeonImageFilters.h"

enum class RifFilterType
{
	BilateralDenoise,
	LwrDenoise,
	EawDenoise,
	MlDenoise,
	MlDenoiseColorOnly
};

enum RifFilterInput
{
	RifColor,
	RifNormal,
	RifDepth,
	RifWorldCoordinate,
	RifObjectId,
	RifTrans,
	RifAlbedo,
	RifMaxInput
};

enum class RifParamType
{
	RifInt,
	RifFloat
};

union RifData
{
	rif_int   i;
	rif_float f;
};

struct RifParam
{
	RifParamType mType;
	RifData      mData;
};

class RifContextWrapper;
class RifFilterWrapper;

class ImageFilter final
{
	TUniquePtr<RifContextWrapper> mRifContext;
	TUniquePtr<RifFilterWrapper>  mRifFilter;

	uint32  mWidth;
	uint32  mHeight;
	FString mModelsPath;

public:
	~ImageFilter();

	int Initialize(const rpr_context rprContext, uint32 width, uint32 height, const FString& modelsPath = FString());

	int CreateFilter(RifFilterType rifFilteType, bool useOpenImageDenoise = false);
	int DeleteFilter();

	int AddInput(RifFilterInput inputId, const rpr_framebuffer rprFrameBuffer, float sigma);
	int AddInput(RifFilterInput inputId, float* memPtr, size_t size, float sigma);
	int AddParam(FString name, RifParam param);

	int AttachFilter();

	int Run();

	int GetData(TArray<float>* data);

private:
	template <class Filter, class... Args>
	int ConstructFilter(Args&&... args)
	{
		int status;

		auto filter = MakeUnique<Filter>();

		status = filter->Initialize(std::forward<Args>(args)...);
		if (status != RIF_SUCCESS)
			return status;

		mRifFilter = std::move(filter);

		return RIF_SUCCESS;
	}
};

enum class RifContextType
{
	RifContextCPU,
	RifContextGPU,
	RifContextGPUMetal
};

class RifContextWrapper
{
protected:
	rif_context mRifContextHandle = nullptr;
	rif_command_queue mRifCommandQueueHandle = nullptr;
	rif_image mOutputRifImage = nullptr;
	RifContextType mContextType = RifContextType::RifContextCPU;

public:
	virtual ~RifContextWrapper();

	const rif_context Context() const;
	const rif_command_queue Queue() const;
	const rif_image Output() const;
	const RifContextType ContextType() const;
	
	int UpdateInputs(const RifFilterWrapper* rifFilter);
	int CreateOutput(const rif_image_desc& desc);

	virtual int Initialize(rpr_context rprContext) = 0;
	virtual int CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* image) = 0;

protected:
	int GetRprCachePath(rpr_context rprContext, TArray<rpr_char>* cachePath);
};

class RifContextGPU final : public RifContextWrapper
{
	const rif_backend_api_type rifBackendApiType = RIF_BACKEND_API_OPENCL;
public:
	int Initialize(rpr_context rprContext) override;
	int CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* outImage) override;
};

class RifContextGPUMetal final : public RifContextWrapper
{
	const rif_backend_api_type rifBackendApiType = RIF_BACKEND_API_METAL;

public:
	int Initialize(const rpr_context rprContext) override;
	int CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* outImage) override;
};

class RifContextCPU final : public RifContextWrapper
{
	const rif_backend_api_type rifBackendApiType = RIF_BACKEND_API_OPENCL;

public:
	int Initialize(rpr_context rprContext);

	int CreateRifImage(const rpr_framebuffer rprFrameBuffer, const rif_image_desc& desc, rif_image* outImage) override;
};

struct RifInput
{
	rif_image mRifImage = nullptr;
	float     mSigma = 0.0f;

	RifInput(rif_image rifImage, float sigma);
	virtual ~RifInput();
	virtual	int Update() = 0;
};

using RifInputPtr = TSharedPtr<RifInput>;

struct RifInputGPU : public RifInput
{
	RifInputGPU(rif_image rifImage, float sigma);
	int Update() override { return RIF_SUCCESS; }
};

struct RifInputGPUCPU : public RifInput
{
	rpr_framebuffer mRprFrameBuffer = nullptr;

	RifInputGPUCPU(rif_image rifImage, const rpr_framebuffer rprFrameBuffer, float sigma);
	int Update() override;
};

struct RifInputCPU : public RifInput
{
	float* mMemPtr = nullptr;
	size_t mSize = 0;

	RifInputCPU(rif_image rifImage, float* memPtr, size_t size, float sigma);
	int Update() override;
};

class RifFilterWrapper
{
	friend class RifContextWrapper;
	friend class RifContextCPU;

protected:
	rif_image_filter mRifImageFilterHandle = nullptr;

	TArray<rif_image_filter> mAuxFilters;
	TArray<rif_image> mAuxImages;

	TMap<RifFilterInput, RifInputPtr> mInputs;
	TMap<FString, RifParam> mParams;

public:
	virtual ~RifFilterWrapper();

	int AddInput(RifFilterInput inputId, const rif_image rifImage, float sigma);
	int AddInput(RifFilterInput inputId, const rif_image rifImage, const rpr_framebuffer rprFrameBuffer, float sigma);
	int AddInput(RifFilterInput inputId, const rif_image rifImage, float* memPtr, size_t size, float sigma);
	int AddParam(FString name, RifParam param);

	virtual int AttachFilter(const RifContextWrapper* rifContext) = 0;
	int DetachFilter(const RifContextWrapper* rifContext);

	int ApplyParameters();

protected:
	int SetupVarianceImageFilter(const rif_image_filter inputFilter, const rif_image outVarianceImage);
};

class RifFilterBilateral final : public RifFilterWrapper
{
	// vector representation of inputs is needed to feed library
	TArray<rif_image> inputImages;
	TArray<float> sigmas;

public:
	int Initialize(const RifContextWrapper* rifContext);
	int AttachFilter(const RifContextWrapper* rifContext) override;
};

class RifFilterLwr final : public RifFilterWrapper
{
	enum
	{
		ColorVar,
		NormalVar,
		DepthVar,
		TransVar,
		AuxFilterMax
	};

	enum
	{
		ColorVarianceImage,
		NormalVarianceImage,
		DepthVarianceImage,
		TransVarianceImage,
		AuxImageMax
	};

public:
	int Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height);
	virtual int AttachFilter(const RifContextWrapper* rifContext) override;
};

class RifFilterEaw final : public RifFilterWrapper
{
	enum
	{
		ColorVar,
		Mlaa,
		AuxFilterMax
	};

	enum
	{
		ColorVarianceImage,
		DenoisedOutputImage,
		AuxImageMax
	};

public:
	int Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height);
	int AttachFilter(const RifContextWrapper* rifContext) override;
};

class RifFilterMl final : public RifFilterWrapper
{
	enum
	{
		NormalsRemapFilter,
		DepthRemapFilter,
		OutputResampleFilter,
		AuxFilterMax
	};

	enum
	{
		MlOutputRifImage,
		AuxImageMax
	};

public:
	int Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height, 
		const FString& modelsPath, bool useOpenImageDenoise);

	int AttachFilter(const RifContextWrapper* rifContext) override;
};

class RifFilterMlColorOnly final : public RifFilterWrapper
{
	enum
	{
		OutputResampleFilter,
		AuxFilterMax
	};

	enum
	{
		MlOutputRifImage,
		AuxImageMax
	};

public:
	int Initialize(const RifContextWrapper* rifContext, uint32 width, uint32 height,
		const FString& modelsPath, bool useOpenImageDenoise);
	int AttachFilter(const RifContextWrapper* rifContext) override;
};
