#pragma once
#ifndef RPRI_GENERIC_API_H_
#define RPRI_GENERIC_API_H_ 1
#include <cassert>
namespace rpri {
	namespace generic {

		struct IAsset
		{
			struct NamedAttribute
			{
				std::string id;
				std::string name;
			};

			virtual char const * GetId() const = 0;

			virtual size_t GetNumberOfAttributes() const = 0;

			virtual NamedAttribute GetAttribureAtIndex(size_t _index) const = 0;

		};

		struct IMaterialNodeMux
		{
		public:
			virtual char const * GetId() const = 0;

			virtual bool IsEmpty() const = 0;

			virtual bool IsNode() const = 0;

			virtual struct IMaterialValue const * GetAsValue() const = 0;
			virtual struct IMaterialNode const * GetAsNode() const = 0;

			virtual ~IMaterialNodeMux() = default;
		};

		struct IMaterialValue
		{
			virtual char const * GetId() const = 0;

			enum class ValueType
			{
				Uint8,
				Uint16,
				Uint32,
				Uint64,
				Int8,
				Int16,
				Int32,
				Int64,
				Float,
				Double,
				String
			};
			virtual ValueType GetType() const = 0;
			virtual size_t GetNumberOfValues() const = 0;

			virtual uint8_t GetValueAtIndexAsUint8(size_t _index) const = 0;
			virtual uint16_t GetValueAtIndexAsUint16(size_t _index) const = 0;
			virtual uint32_t GetValueAtIndexAsUint32(size_t _index) const = 0;
			virtual uint64_t GetValueAtIndexAsUint64(size_t _index) const = 0;
			virtual int8_t GetValueAtIndexAsInt8(size_t _index) const = 0;
			virtual int16_t GetValueAtIndexAsInt16(size_t _index) const = 0;
			virtual int32_t GetValueAtIndexAsInt32(size_t _index) const = 0;
			virtual int64_t GetValueAtIndexAsInt64(size_t _index) const = 0;
			virtual float GetValueAtIndexAsFloat(size_t _index) const = 0;
			virtual double GetValueAtIndexAsDouble(size_t _index) const = 0;
			virtual std::string GetValueAsString() const = 0;

			virtual char const * GetMetadata() const = 0;

			virtual ~IMaterialValue() = default;
		};

		struct IMaterialNode
		{
			virtual char const * GetId() const = 0;

			virtual char const * GetName() const = 0;
			virtual char const * GetType() const = 0;
			virtual size_t GetNumberOfInputs() const = 0;
			virtual IMaterialNodeMux const * GetInputAt(size_t _index) const = 0;
			virtual char const * GetInputNameAt(size_t _index) const = 0;

			// some nodes take a texture input (the *Map set + TextureTexelSize)
			// this allow access to it
			virtual bool HasTextureInput() const = 0;
			virtual struct ITexture const * GetTextureInput() const = 0;

			virtual char const * GetMetadata() const = 0;
			virtual ~IMaterialNode() = default;
		};

		// currently used for generic import
		struct IMaterialGraph
		{
			virtual char const * GetId() const = 0;
			virtual char const * GetName() const = 0;

			virtual size_t GetNumberOfMaterialValues() const = 0;
			virtual IMaterialValue const * GetMaterialValueAt(size_t _index) const = 0;

			virtual size_t GetNumberOfMaterialNodes() const = 0;
			virtual IMaterialNode const * GetMaterialNodeAt(size_t _index) const = 0;

			virtual char const * GetMetadata() const = 0;
			virtual IMaterialNode const * GetRootNode() const = 0;
		};


		struct IMeshPart
		{
			enum class PrimitiveType
			{
				Points,
				Lines,
				Triangles,
				LineLoop,
				TriangleStrip,
				TriangleFan
			};
			enum class VertexType
			{
				Position = 0,
				Normal,
				Tangent,
				Colour,
				TexCoord0,
				TexCoord1,
				TexCoord2,
				TexCoord3,
				TexCoord4,
				TexCoord5,
				TexCoord6,
				TexCoord7,
			};

			struct VertexData
			{
				union
				{
					float f[4];
				};
			};

			virtual char const * GetId() const = 0;

			virtual PrimitiveType GetPrimitiveType() const = 0;
			virtual size_t GetNumberOfIndices() const = 0;
			virtual uint32_t GetIndexAt(size_t _index) const = 0;
			virtual size_t GetNumberOfIndicesAsWholePrimitives() const = 0;
			virtual uint32_t GetIndexAsWholePrimitivesAt(size_t _index) const = 0;

			virtual size_t GetNumberOfVerticesOfType(VertexType _type) const = 0;
			virtual VertexData GetVertexOfTypeAt(VertexType _type, size_t _vindex) const = 0;
			virtual VertexData GetVertexOfTypeAtIndexOf(VertexType _type, size_t _index) const = 0;
			virtual VertexData GetVertexOfTypeAtIndexOfWholePrimitive(VertexType _type, size_t _index) const = 0;

			virtual IMaterialNode const * GetMaterialRootNode() const = 0;
			virtual char const * GetMaterialName() const = 0;

			virtual char const * GetMetadata() const = 0;

			virtual ~IMeshPart() = default;
		};

		struct IMesh
		{
			virtual char const * GetId() const = 0;

			virtual char const * GetName() const = 0;
			virtual size_t GetNumberOfParts() const = 0;
			virtual IMeshPart const * GetPartAt(size_t _index) const = 0;

			virtual char const * GetMetadata() const = 0;

			virtual ~IMesh() = default;
		};

		struct ISceneNode
		{
			virtual char const * GetId() const = 0;

			virtual char const * GetName() const = 0;

			virtual size_t GetNumberOfChildren() const = 0;
			virtual ISceneNode const * GetChildAt(size_t _index) const = 0;

			virtual size_t GetNumberOfMeshes() const = 0;
			virtual IMesh const * GetMeshAt(size_t _index) const = 0;

			// todo expose TRS transform as well as 4x4 matrix
			virtual void Get4x4TransformMatrix(float *_matrix) const = 0;

			virtual char const * GetMetadata() const = 0;

			virtual ~ISceneNode() = default;

		};

		struct IScene
		{
			virtual char const * GetId() const = 0;

			virtual size_t GetNumberOfSceneNodes() const = 0;
			virtual ISceneNode const * GetSceneNodeAt(size_t _index) const = 0;

			virtual char const * GetMetadata() const = 0;

			virtual ~IScene() = default;
		};

		struct IImage
		{
			enum class ColourSpace
			{
				sRGB = 0,
				Linear = 1,
			};

			enum class ComponentFormat
			{
				Uint8 = 0,
				Float = 1,

				Unknown = 0xff,
			};

			virtual char const * GetId() const = 0;

			virtual size_t GetWidth() const = 0;
			virtual size_t GetHeight() const = 0;
			virtual size_t GetDepth() const = 0;
			virtual size_t GetSlices() const = 0;
			virtual size_t GetNumberofComponents() const = 0;

			virtual ComponentFormat GetComponentFormat(size_t _index) const = 0;
			virtual ColourSpace GetColourSpace() const = 0;

			virtual size_t GetPixelSizeInBits() const = 0;
			virtual size_t GetRowStrideInBits() const = 0;

			virtual size_t GetRawSizeInBytes() const = 0;
			virtual uint8_t const * GetRawByteData() const = 0;
			virtual float const * GetRawFloatData() const = 0;

			virtual std::string GetOriginalPath() const = 0;

			// TODO better extraction function for different bitwidths, packed etc.
			virtual float GetComponent2DAsFloat(size_t _x, size_t _y, size_t _comp) const = 0;
			virtual uint8_t GetComponent2DAsUint8(size_t _x, size_t _y, size_t _comp) const = 0;


			// optional 'bulk' texture conversion, false if not supported  
			virtual bool GetBulk2DAsFloats(float * /*_dest*/) const { return false; }
			virtual bool GetBulk2DAsUint8s(uint8_t * /*_dest*/) const { return false; }

			static constexpr size_t GetComponentBitWidth(ComponentFormat _format)
			{
				return	(_format == ComponentFormat::Uint8) ? sizeof(uint8_t) * 8 :
					(_format == ComponentFormat::Float) ? sizeof(float) :
					0;
			}

			virtual char const * GetMetadata() const = 0;
		};

		struct ISampler
		{
			enum class FilterType
			{
				Point,
				Linear,
				PointMipMapPoint,
				LinearMipMapPoint,
				PointMipMapLinear,
				LinearMipMapLinear,
				Anistropic,
			};

			enum class WrapType
			{
				Repeat,
				ClampToEdge,
				MirrorRepeat
			};

			virtual char const * GetId() const = 0;

			virtual FilterType GetMinFilter() const = 0;
			virtual FilterType GetMagFilter() const = 0;

			virtual WrapType GetWrapS() const = 0;
			virtual WrapType GetWrapT() const = 0;
			virtual WrapType GetWrapR() const = 0;

			virtual char const * GetMetadata() const = 0;

		};

		struct ITexture
		{
			enum class TextureType
			{
				General = 0,
				Alpha,
				RGB,
				RGBA,
				Luminance,
				LuminanceAlpha,
				Normal,
				Bump,
				Occlusion,
				Displacement
			};

			virtual char const * GetId() const = 0;

			virtual ISampler const * GetSampler() const = 0;

			virtual IImage const * GetImage() const = 0;

			virtual bool GetImageYUp() const = 0;

			virtual ITexture::TextureType GetTextureType() const = 0;

			virtual char const * GetMetadata() const = 0;

		};
	}
} // end namespace rpri::generic

#endif // endif RPRI_GENERIC_API_H_