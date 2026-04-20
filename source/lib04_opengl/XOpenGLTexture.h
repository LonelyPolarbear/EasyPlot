#pragma once
#include "XOpenglApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataArray.h>
#include "XOpenGLType.h"

class XOpenGLBuffer;
/**
 * @class				XOpenGLBuffer
 * @brief				纹理对象
 * @details			纹理对象是OpenGL中的一种资源对象，用于存储和操作图像数据。
 *							包含纹理创建 数据写入 数据回读 纹理销毁等操作
 * @author			宋伟军
 * @date				2026-04-20
 * @version			1.0
 * @warning		只允许在当前上下文所在的线程使用
 */
class LIB04_OPENGL_API XOpenGLTexture :public XDataBaseObject{
public:
	enum Target {
		Target1D = 0x0DE0,									///<  GL_TEXTURE_1D
		Target1DArray = 0x8C18,							///<  GL_TEXTURE_1D_ARRAY
		Target2D = 0x0DE1,									///<  GL_TEXTURE_2D
		Target2DArray = 0x8C1A,							///<  GL_TEXTURE_2D_ARRAY
		Target3D = 0x806F,									///<  GL_TEXTURE_3D
		TargetCubeMap = 0x8513,							///<  GL_TEXTURE_CUBE_MAP
		TargetCubeMapArray = 0x9009,				///<  GL_TEXTURE_CUBE_MAP_ARRAY
		Target2DMultisample = 0x9100,				///<  GL_TEXTURE_2D_MULTISAMPLE
		Target2DMultisampleArray = 0x9102,		///<  GL_TEXTURE_2D_MULTISAMPLE_ARRAY
		TargetRectangle = 0x84F5,						///<  GL_TEXTURE_RECTANGLE
		TargetBuffer = 0x8C2A								///<  GL_TEXTURE_BUFFER
	};

	enum MipMapGeneration {
		GenerateMipMaps,
		DontGenerateMipMaps
	};

	enum TextureUnitReset {
		ResetTextureUnit,
		DontResetTextureUnit
	};

	//纹理内部格式
	enum TextureFormat {
		NoFormat = 0,         // GL_NONE

		// Unsigned normalized formats
		R8_UNorm = 0x8229,    // GL_R8
		RG8_UNorm = 0x822B,    // GL_RG8
		RGB8_UNorm = 0x8051,    // GL_RGB8
		RGBA8_UNorm = 0x8058,    // GL_RGBA8

		R16_UNorm = 0x822A,    // GL_R16
		RG16_UNorm = 0x822C,    // GL_RG16
		RGB16_UNorm = 0x8054,    // GL_RGB16
		RGBA16_UNorm = 0x805B,    // GL_RGBA16

		// Signed normalized formats
		R8_SNorm = 0x8F94,    // GL_R8_SNORM
		RG8_SNorm = 0x8F95,    // GL_RG8_SNORM
		RGB8_SNorm = 0x8F96,    // GL_RGB8_SNORM
		RGBA8_SNorm = 0x8F97,    // GL_RGBA8_SNORM

		R16_SNorm = 0x8F98,    // GL_R16_SNORM
		RG16_SNorm = 0x8F99,    // GL_RG16_SNORM
		RGB16_SNorm = 0x8F9A,    // GL_RGB16_SNORM
		RGBA16_SNorm = 0x8F9B,    // GL_RGBA16_SNORM

		// Unsigned integer formats
		R8U = 0x8232,    // GL_R8UI
		RG8U = 0x8238,    // GL_RG8UI
		RGB8U = 0x8D7D,    // GL_RGB8UI
		RGBA8U = 0x8D7C,    // GL_RGBA8UI

		R16U = 0x8234,    // GL_R16UI
		RG16U = 0x823A,    // GL_RG16UI
		RGB16U = 0x8D77,    // GL_RGB16UI
		RGBA16U = 0x8D76,    // GL_RGBA16UI

		R32U = 0x8236,    // GL_R32UI
		RG32U = 0x823C,    // GL_RG32UI
		RGB32U = 0x8D71,    // GL_RGB32UI
		RGBA32U = 0x8D70,    // GL_RGBA32UI

		// Signed integer formats
		R8I = 0x8231,    // GL_R8I
		RG8I = 0x8237,    // GL_RG8I
		RGB8I = 0x8D8F,    // GL_RGB8I
		RGBA8I = 0x8D8E,    // GL_RGBA8I

		R16I = 0x8233,    // GL_R16I
		RG16I = 0x8239,    // GL_RG16I
		RGB16I = 0x8D89,    // GL_RGB16I
		RGBA16I = 0x8D88,    // GL_RGBA16I

		R32I = 0x8235,    // GL_R32I
		RG32I = 0x823B,    // GL_RG32I
		RGB32I = 0x8D83,    // GL_RGB32I
		RGBA32I = 0x8D82,    // GL_RGBA32I

		// Floating point formats
		R16F = 0x822D,    // GL_R16F
		RG16F = 0x822F,    // GL_RG16F
		RGB16F = 0x881B,    // GL_RGB16F
		RGBA16F = 0x881A,    // GL_RGBA16F

		R32F = 0x822E,    // GL_R32F
		RG32F = 0x8230,    // GL_RG32F
		RGB32F = 0x8815,    // GL_RGB32F
		RGBA32F = 0x8814,    // GL_RGBA32F

		// Packed formats
		RGB9E5 = 0x8C3D,    // GL_RGB9_E5
		RG11B10F = 0x8C3A,    // GL_R11F_G11F_B10F
		RG3B2 = 0x2A10,    // GL_R3_G3_B2
		R5G6B5 = 0x8D62,    // GL_RGB565
		RGB5A1 = 0x8057,    // GL_RGB5_A1
		RGBA4 = 0x8056,    // GL_RGBA4
		RGB10A2 = 0x906F,    // GL_RGB10_A2UI

		// Depth formats
		D16 = 0x81A5,    // GL_DEPTH_COMPONENT16
		D24 = 0x81A6,    // GL_DEPTH_COMPONENT24
		D24S8 = 0x88F0,    // GL_DEPTH24_STENCIL8
		D32 = 0x81A7,    // GL_DEPTH_COMPONENT32
		D32F = 0x8CAC,    // GL_DEPTH_COMPONENT32F
		D32FS8X24 = 0x8CAD,    // GL_DEPTH32F_STENCIL8
		S8 = 0x8D48,    // GL_STENCIL_INDEX8

		// Compressed formats
		RGB_DXT1 = 0x83F0,    // GL_COMPRESSED_RGB_S3TC_DXT1_EXT
		RGBA_DXT1 = 0x83F1,    // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
		RGBA_DXT3 = 0x83F2,    // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
		RGBA_DXT5 = 0x83F3,    // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
		R_ATI1N_UNorm = 0x8DBB,    // GL_COMPRESSED_RED_RGTC1
		R_ATI1N_SNorm = 0x8DBC,    // GL_COMPRESSED_SIGNED_RED_RGTC1
		RG_ATI2N_UNorm = 0x8DBD,    // GL_COMPRESSED_RG_RGTC2
		RG_ATI2N_SNorm = 0x8DBE,    // GL_COMPRESSED_SIGNED_RG_RGTC2
		RGB_BP_UNSIGNED_FLOAT = 0x8E8F,    // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
		RGB_BP_SIGNED_FLOAT = 0x8E8E,    // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
		RGB_BP_UNorm = 0x8E8C,    // GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
		R11_EAC_UNorm = 0x9270,    // GL_COMPRESSED_R11_EAC
		R11_EAC_SNorm = 0x9271,    // GL_COMPRESSED_SIGNED_R11_EAC
		RG11_EAC_UNorm = 0x9272,    // GL_COMPRESSED_RG11_EAC
		RG11_EAC_SNorm = 0x9273,    // GL_COMPRESSED_SIGNED_RG11_EAC
		RGB8_ETC2 = 0x9274,    // GL_COMPRESSED_RGB8_ETC2
		SRGB8_ETC2 = 0x9275,    // GL_COMPRESSED_SRGB8_ETC2
		RGB8_PunchThrough_Alpha1_ETC2 = 0x9276, // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
		SRGB8_PunchThrough_Alpha1_ETC2 = 0x9277, // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
		RGBA8_ETC2_EAC = 0x9278,    // GL_COMPRESSED_RGBA8_ETC2_EAC
		SRGB8_Alpha8_ETC2_EAC = 0x9279,    // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
		RGB8_ETC1 = 0x8D64,    // GL_ETC1_RGB8_OES
		RGBA_ASTC_4x4 = 0x93B0,    // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
		RGBA_ASTC_5x4 = 0x93B1,    // GL_COMPRESSED_RGBA_ASTC_5x4_KHR
		RGBA_ASTC_5x5 = 0x93B2,    // GL_COMPRESSED_RGBA_ASTC_5x5_KHR
		RGBA_ASTC_6x5 = 0x93B3,    // GL_COMPRESSED_RGBA_ASTC_6x5_KHR
		RGBA_ASTC_6x6 = 0x93B4,    // GL_COMPRESSED_RGBA_ASTC_6x6_KHR
		RGBA_ASTC_8x5 = 0x93B5,    // GL_COMPRESSED_RGBA_ASTC_8x5_KHR
		RGBA_ASTC_8x6 = 0x93B6,    // GL_COMPRESSED_RGBA_ASTC_8x6_KHR
		RGBA_ASTC_8x8 = 0x93B7,    // GL_COMPRESSED_RGBA_ASTC_8x8_KHR
		RGBA_ASTC_10x5 = 0x93B8,    // GL_COMPRESSED_RGBA_ASTC_10x5_KHR
		RGBA_ASTC_10x6 = 0x93B9,    // GL_COMPRESSED_RGBA_ASTC_10x6_KHR
		RGBA_ASTC_10x8 = 0x93BA,    // GL_COMPRESSED_RGBA_ASTC_10x8_KHR
		RGBA_ASTC_10x10 = 0x93BB,    // GL_COMPRESSED_RGBA_ASTC_10x10_KHR
		RGBA_ASTC_12x10 = 0x93BC,    // GL_COMPRESSED_RGBA_ASTC_12x10_KHR
		RGBA_ASTC_12x12 = 0x93BD,    // GL_COMPRESSED_RGBA_ASTC_12x12_KHR
		SRGB8_Alpha8_ASTC_4x4 = 0x93D0,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
		SRGB8_Alpha8_ASTC_5x4 = 0x93D1,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
		SRGB8_Alpha8_ASTC_5x5 = 0x93D2,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
		SRGB8_Alpha8_ASTC_6x5 = 0x93D3,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
		SRGB8_Alpha8_ASTC_6x6 = 0x93D4,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
		SRGB8_Alpha8_ASTC_8x5 = 0x93D5,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
		SRGB8_Alpha8_ASTC_8x6 = 0x93D6,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
		SRGB8_Alpha8_ASTC_8x8 = 0x93D7,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
		SRGB8_Alpha8_ASTC_10x5 = 0x93D8,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
		SRGB8_Alpha8_ASTC_10x6 = 0x93D9,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
		SRGB8_Alpha8_ASTC_10x8 = 0x93DA,    // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
		SRGB8_Alpha8_ASTC_10x10 = 0x93DB,   // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
		SRGB8_Alpha8_ASTC_12x10 = 0x93DC,   // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
		SRGB8_Alpha8_ASTC_12x12 = 0x93DD,   // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR

		// sRGB formats
		SRGB8 = 0x8C41,    // GL_SRGB8
		SRGB8_Alpha8 = 0x8C43,    // GL_SRGB8_ALPHA8
		SRGB_DXT1 = 0x8C4C,    // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
		SRGB_Alpha_DXT1 = 0x8C4D,    // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
		SRGB_Alpha_DXT3 = 0x8C4E,    // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
		SRGB_Alpha_DXT5 = 0x8C4F,    // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
		SRGB_BP_UNorm = 0x8E8D   // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
	};

	enum TextureFormatClass {
		NoFormatClass,
		FormatClass_128Bit,
		FormatClass_96Bit,
		FormatClass_64Bit,
		FormatClass_48Bit,
		FormatClass_32Bit,
		FormatClass_24Bit,
		FormatClass_16Bit,
		FormatClass_8Bit,
		FormatClass_RGTC1_R,
		FormatClass_RGTC2_RG,
		FormatClass_BPTC_Unorm,
		FormatClass_BPTC_Float,
		FormatClass_S3TC_DXT1_RGB,
		FormatClass_S3TC_DXT1_RGBA,
		FormatClass_S3TC_DXT3_RGBA,
		FormatClass_S3TC_DXT5_RGBA,
		FormatClass_Unique
	};

	enum CubeMapFace {
		CubeMapPositiveX = 0x8515,  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
		CubeMapNegativeX = 0x8516,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		CubeMapPositiveY = 0x8517,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		CubeMapNegativeY = 0x8518,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		CubeMapPositiveZ = 0x8519,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		CubeMapNegativeZ = 0x851A   // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	enum PixelFormat {
		NoSourceFormat = 0,         // GL_NONE
		Red = 0x1903,    // GL_RED
		RG = 0x8227,    // GL_RG
		RGB = 0x1907,    // GL_RGB
		BGR = 0x80E0,    // GL_BGR
		RGBA = 0x1908,    // GL_RGBA
		BGRA = 0x80E1,    // GL_BGRA
		Red_Integer = 0x8D94,    // GL_RED_INTEGER
		RG_Integer = 0x8228,    // GL_RG_INTEGER
		RGB_Integer = 0x8D98,    // GL_RGB_INTEGER
		BGR_Integer = 0x8D9A,    // GL_BGR_INTEGER
		RGBA_Integer = 0x8D99,    // GL_RGBA_INTEGER
		BGRA_Integer = 0x8D9B,    // GL_BGRA_INTEGER
		Stencil = 0x1901,    // GL_STENCIL_INDEX
		Depth = 0x1902,    // GL_DEPTH_COMPONENT
		DepthStencil = 0x84F9,    // GL_DEPTH_STENCIL
		Alpha = 0x1906,    // GL_ALPHA
		Luminance = 0x1909,    // GL_LUMINANCE
		LuminanceAlpha = 0x190A     // GL_LUMINANCE_ALPHA
	};

	enum PixelType {
		NoPixelType = 0,         // GL_NONE
		Int8 = 0x1400,    // GL_BYTE
		UInt8 = 0x1401,    // GL_UNSIGNED_BYTE
		Int16 = 0x1402,    // GL_SHORT
		UInt16 = 0x1403,    // GL_UNSIGNED_SHORT
		Int32 = 0x1404,    // GL_INT
		UInt32 = 0x1405,    // GL_UNSIGNED_INT
		Float16 = 0x140B,    // GL_HALF_FLOAT
		Float16OES = 0x8D61,    // GL_HALF_FLOAT_OES
		Float32 = 0x1406,    // GL_FLOAT
		UInt32_RGB9_E5 = 0x8C3E,    // GL_UNSIGNED_INT_5_9_9_9_REV
		UInt32_RG11B10F = 0x8C3B,    // GL_UNSIGNED_INT_10F_11F_11F_REV
		UInt8_RG3B2 = 0x8032,    // GL_UNSIGNED_BYTE_3_3_2
		UInt8_RG3B2_Rev = 0x8362,    // GL_UNSIGNED_BYTE_2_3_3_REV
		UInt16_RGB5A1 = 0x8034,    // GL_UNSIGNED_SHORT_5_5_5_1
		UInt16_RGB5A1_Rev = 0x8366,    // GL_UNSIGNED_SHORT_1_5_5_5_REV
		UInt16_R5G6B5 = 0x8363,    // GL_UNSIGNED_SHORT_5_6_5
		UInt16_R5G6B5_Rev = 0x8364,    // GL_UNSIGNED_SHORT_5_6_5_REV
		UInt16_RGBA4 = 0x8033,    // GL_UNSIGNED_SHORT_4_4_4_4
		UInt16_RGBA4_Rev = 0x8365,    // GL_UNSIGNED_SHORT_4_4_4_4_REV
		UInt32_RGBA8 = 0x8035,    // GL_UNSIGNED_INT_8_8_8_8
		UInt32_RGBA8_Rev = 0x8367,    // GL_UNSIGNED_INT_8_8_8_8_REV
		UInt32_RGB10A2 = 0x8036,    // GL_UNSIGNED_INT_10_10_10_2
		UInt32_RGB10A2_Rev = 0x8368,    // GL_UNSIGNED_INT_2_10_10_10_REV
		UInt32_D24S8 = 0x84FA,    // GL_UNSIGNED_INT_24_8
		Float32_D32_UInt32_S8_X24 = 0x8DAD // GL_FLOAT_32_UNSIGNED_INT_24_8_REV
	};

	enum WrapMode {
		Repeat = 0x2901, // GL_REPEAT
		MirroredRepeat = 0x8370, // GL_MIRRORED_REPEAT
		ClampToEdge = 0x812F, // GL_CLAMP_TO_EDGE
		ClampToBorder = 0x812D  // GL_CLAMP_TO_BORDER
	};

	enum CoordinateDirection {
		DirectionS = 0x2802, // GL_TEXTURE_WRAP_S
		DirectionT = 0x2803, // GL_TEXTURE_WRAP_T
		DirectionR = 0x8072  // GL_TEXTURE_WRAP_R
	};

	enum Filter {
		Nearest = 0x2600,   // GL_NEAREST
		Linear = 0x2601,   // GL_LINEAR
		NearestMipMapNearest = 0x2700,   // GL_NEAREST_MIPMAP_NEAREST
		NearestMipMapLinear = 0x2702,   // GL_NEAREST_MIPMAP_LINEAR
		LinearMipMapNearest = 0x2701,   // GL_LINEAR_MIPMAP_NEAREST
		LinearMipMapLinear = 0x2703    // GL_LINEAR_MIPMAP_LINEAR
	};

	enum DepthStencilMode {
		DepthMode = 0x1902,   // GL_DEPTH_COMPONENT
		StencilMode = 0x1901    // GL_STENCIL_INDEX
	};

	enum ComparisonFunction {
		CompareLessEqual = 0x0203,   // GL_LEQUAL
		CompareGreaterEqual = 0x0206,   // GL_GEQUAL
		CompareLess = 0x0201,   // GL_LESS
		CompareGreater = 0x0204,   // GL_GREATER
		CompareEqual = 0x0202,   // GL_EQUAL
		CommpareNotEqual = 0x0205,   // GL_NOTEQUAL
		CompareAlways = 0x0207,   // GL_ALWAYS
		CompareNever = 0x0200    // GL_NEVER
	};

public:
	XOpenGLTexture();
	~XOpenGLTexture();

	/**
	 * @brief 设置纹理的目标
	*/
	void setTarget(Target target);

	/**
	 * @brief 创建纹理
	 */
	bool create();

	/**
	 * @brief 设置纹理的内部格式,建议纹理格式在整个生命周期值设置一次
	 */
	void setInternalFormat(TextureFormat format);

	/**
	 * @brief 设置纹理的输入数据格式,建议输入数据格式在整个生命周期值设置一次
	*/
	void setExternalFormat( PixelFormat inputDataFormat, PixelType inputDataPixelType);

	/**
	 * @brief 一维纹理分配内存分配大小 width *internalDataSize,如果当前大小为0，或者不等于预分配大小，则重新分配，否则直接返回，避免多次分配
	 */
	void texStorage1D(int width);

	/**
	 * @brief 二维纹理分配内存 Target1DArray、Target2D
	 */
	void texStorage2D(int width, int height);

	/**
	 * @brief 二维多重采样纹理分配内存
	 */
	void texStorage2DMultiSample(int width, int height,int sampleNum);

	/**
	 * @brief 三维纹理分配内存 Target2DArray、Target3D
	 */
	void texStorage3D(int width, int height, int depth);

	/**
	 * @brief 三维多重采样纹理分配内存
	 */
	void texStorage3DMultiSample(int width, int height, int depth, int sampleNum);

	/**
	* @brief 填充一维纹理数据，不进行大小检查，用户应该保证大小
	*/
	void setSubData1D(int xoffset, int width, const void* data);

	/**
	* @brief 填充二维纹理数据 Target1DArray、Target2D
	*/
	void setSubData2D(int xoffset, int yoffset, int width, int height, const void* data);

	/**
	* @brief 填充三维纹理数据 Target2DArray、Target3D
	*/
	void setSubData3D(int xoffset, int yoffset, int zoffset, int width, int height, int depth, const void* data);

	/**
	 * @brief					利用fbo 读取一维纹理数据,用于Target1D、Target1DArray
	 * @param[in]			xoffset			偏移量，如果读取的是一维数组纹理，则表示的是当前层的偏移
	 * @param[in]			width			读取的纹素个数
	 * @param[in]			level				mipmap层级
	 * @param[in]			layer			要读取的一维纹理数组的当前层
	 */
	sptr<XUCharArray> getSubData1D(int xoffset, int width, int level=0, int layer =0);

	/**
	 * @brief 利用fbo 读取二维纹理数据 用于Target1DArray、Target2D、Target2DArray
	 */
	sptr<XUCharArray2D> getSubData2D(int xoffset, int yoffset, int width, int height, int level =0, int layer=0);

	/**
	 * @brief 利用fbo 读取三维纹理数据 Target2DArray Target3D
	 */
	sptr<XUCharArray3D> getSubData3D(int xoffset, int yoffset, int zoffset, int width, int height, int depth);

	/**
	* @brief 读取纹理数据到pbo
	*/
	sptr<XOpenGLBuffer> map2pbo();

	/**
	* @brief 读取多重采样纹理数据到pbo
	*/
	sptr<XOpenGLBuffer> mapMultiSample2pbo(unsigned int attachment);

	/**
	* @brief 获取纹理目标类型
	*/
	Target getTarget() const;

	/**
	* @brief 获取纹理内部ID
	*/
	uint32_t getId() const;

	/**
	* @brief 获取纹理内部格式
	*/
	XOpenGLTexture::TextureFormat getInternalFormat() const;

	/**
	* @brief 获取纹理输入数据Format
	*/
	XOpenGLTexture::PixelFormat getInputDataPixelFormat() const;

	/**
	* @brief 获取纹理输入数据类型
	*/
	XOpenGLTexture::PixelType getInputDataPixelType() const;

	/**
	* @brief gl底层纹理删除
	*/
	void destroy();

	/**
	* @brief 绑定纹理
	*/
	void bind();

	/**
	* @brief 绑定纹理单元
	*/
	void bindUnit(unsigned int unit);

	/**
	* @brief 解绑纹理
	*/
	void release();

	void releaseUnit(unsigned int unit);

	void bindBuffer(std::shared_ptr<XOpenGLBuffer> buffer, TextureFormat format);

	/**
	* @brief 设置放大时的过滤方式
	*/
	void setMagnificationFilter(XOpenGLTexture::Filter filter);

	/**
	* @brief 设置缩小时的过滤方式
	*/
	void setMinificationFilter(XOpenGLTexture::Filter filter);

	/**
	* @brief 设置纹理的环绕方式
	*/
	void setWrapMode(XOpenGLTexture::CoordinateDirection direction, XOpenGLTexture::WrapMode mode);

	/**
	* @brief 打印纹理内容
	*/
	std::string dump() override;

	/**
	* @brief 返回纹理内部单个像素占据的内存字节数量
	*/
	static unsigned int getInternalFormatSize(XOpenGLTexture::TextureFormat format);

	/**
	* @brief 对于指定的外部纹理格式和像素类型，返回每个像素占据的字节数
	*/
	static unsigned int getExternalPerpixelSize(XOpenGLTexture::PixelFormat format, XOpenGLTexture::PixelType type);

	XOpenGL::TextureBindingType getTextureBindType() const;
protected:
	/**
	* @brief 返回当前纹理分配的储存空间大小(字节数)，注意多重采样纹理需要没有乘采样数
	*/
	int getStorgeSize();

	/**
	* @brief 计算给定参数的纹理理论的存储大小(字节数)
	*/
	int computeStorgeSize(int width, int height, int depth, XOpenGLTexture::TextureFormat format,int sampleNum =1);
protected:
	class Internal;
	std::unique_ptr<Internal> d;
};