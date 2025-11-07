#pragma once
#include "XOpenglApi.h"

/// <summary>
/// opengl相关的宏定义
/// </summary>
namespace XOpenGL {
	enum class LIB04_OPENGL_API FrameBufferType :unsigned int {
		readBuffer = 0x8CA8,		    //GL_READ_FRAMEBUFFER
		drawBuffer = 0x8CA9,		    //GL_DRAW_FRAMEBUFFER
		framebuffer = 0x8D40,		//GL_FRAMEBUFFER
	};

	enum class LIB04_OPENGL_API FrameBufferBindingType :unsigned int {
		readBufferBinding = 0x8CAA,									//GL_READ_FRAMEBUFFER_BINDING
		drawBufferBinding = 0x8CAB,									//GL_DRAW_FRAMEBUFFER_BINDING
		framebufferBinding = 0x8CA6,									//GL_FRAMEBUFFER_BINDING
		//GL_NONE

	};

	enum class LIB04_OPENGL_API DataBufferBindingType :unsigned int {
		invalidBinding = 0,
		vertexBufferBinding = 0x8894,									//GL_ARRAY_BUFFER_BINDING
		elementArrayBufferBinding = 0x8895,						//GL_ELEMENT_ARRAY_BUFFER_BINDING
		pixelPackBufferBinding = 0x88ED,							//GL_PIXEL_PACK_BUFFER_BINDING
		pixelUnpackBufferBinding = 0x88EF,						//GL_PIXEL_UNPACK_BUFFER_BINDING
		transformFeedbackBufferBinding = 0x8C8F,			//GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
		uniformBufferBinding = 0x8A28,								//GL_UNIFORM_BUFFER_BINDING	
		shaderStorageBufferBinding = 0x90D3,					//GL_SHADER_STORAGE_BUFFER_BINDING
	};

	enum class LIB04_OPENGL_API TextureBindingType :unsigned int {
		XGL_TEXTURE_BINDING_1D = 0x8068,
		XGL_TEXTURE_BINDING_1D_ARRAY = 0x8C18,

		XGL_TEXTURE_BINDING_2D = 0x8069,
		XGL_TEXTURE_BINDING_2D_ARRAY = 0x8C1D,

		XGL_TEXTURE_BINDING_3D = 0x806A,

		XGL_TEXTURE_BINDING_CUBE_MAP = 0x8514,
		XGL_TEXTURE_BINDING_CUBE_MAP_ARRAY = 0x900A,

		XGL_TEXTURE_BINDING_2D_MULTISAMPLE = 0x9104,
		XGL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY = 0x9105,

		XGL_TEXTURE_BINDING_RECTANGLE = 0x84F6,
		XGL_TEXTURE_BINDING_BUFFER = 0x8C2C,
	};

	/// <summary>
	/// opengl的以_BIT结尾的宏
	/// </summary>
	enum class LIB04_OPENGL_API FlagBits :unsigned int {
		color_buffer_bit = 0x00004000,  //GL_COLOR_BUFFER_BIT
		depth_buffer_bit = 0x00000100,  //GL_DEPTH_BUFFER_BIT
		stencil_buffer_bit = 0x00000400,  //GL_STENCIL_BUFFER_BIT
	};

	enum class LIB04_OPENGL_API BufferBits :unsigned int {
		color_buffer_bit = 0x00004000,  //GL_COLOR_BUFFER_BIT
		depth_buffer_bit = 0x00000100,  //GL_DEPTH_BUFFER_BIT
		stencil_buffer_bit = 0x00000400,  //GL_STENCIL_BUFFER_BIT
	};

	//GL_SCISSOR_TEST
	enum class LIB04_OPENGL_API EnableType :unsigned int {
		XGL_MULTISAMPLE = 0x809D,											//GL_MULTISAMPLE
		XGL_SCISSOR_TEST = 0x0C11,											//GL_SCISSOR_TEST
		XGL_DEPTH_TEST = 0x0B71,												//GL_DEPTH_TEST
		XGL_STENCIL_TEST = 0x0B90,											//GL_STENCIL_TEST
		XGL_CULL_FACE = 0x0B44,												//GL_CULL_FACE
		XGL_BLEND = 0x0BE2,														//GL_BLEND
		XGL_PROGRAM_POINT_SIZE = 0x8642,							//GL_PROGRAM_POINT_SIZE

	};

	enum class LIB04_OPENGL_API FilterType :unsigned int {
		nearest = 0x2600,		//GL_NEAREST
		linear = 0x2601,		//GL_LINEAR
	};

	enum class LIB04_OPENGL_API PixelStoreParameter :unsigned int {
		unpack_swap_bytes = 0x0CF0,	//GL_UNPACK_SWAP_BYTES
		unpack_lsb_first = 0x0CF1,	//GL_UNPACK_LSB_FIRST
		unpack_row_length = 0x0CF2,	//GL_UNPACK_ROW_LENGTH
		unpack_skip_rows = 0x0CF3,	//GL_UNPACK_SKIP_ROWS
		unpack_skip_pixels = 0x0CF4,	//GL_UNPACK_SKIP_PIXELS
		unpack_alignment = 0x0CF5,	//GL_UNPACK_ALIGNMENT
		pack_swap_bytes = 0x0D00,	//GL_PACK_SWAP_BYTES
		pack_lsb_first = 0x0D01,	//GL_PACK_LSB_FIRST
		pack_row_length = 0x0D02,	//GL_PACK_ROW_LENGTH
		pack_skip_rows = 0x0D03,	//GL_PACK_SKIP_ROWS
		pack_skip_pixels = 0x0D04,	//GL_PACK_SKIP_PIXELS
		pack_alignment = 0x0D05,	//GL_PACK_ALIGNMENT
	};

	struct LIB04_OPENGL_API OtherType {
		static const unsigned int sample_buffers = 0x80A8;//GL_SAMPLE_BUFFERS
		static const unsigned int samples = 0x80A9;//GL_SAMPLES
	};

	enum class LIB04_OPENGL_API DataType :unsigned int {
		unsigned_byte = 0x1401,	//GL_UNSIGNED_BYTE
		byte = 0x1400,			//GL_BYTE
		unsigned_short = 0x1403,	//GL_UNSIGNED_SHORT
		short_ = 0x1402,			//GL_SHORT
		unsigned_int = 0x1405,	//GL_UNSIGNED_INT
		int_ = 0x1404,			//GL_INT
		float_ = 0x1406,			//GL_FLOAT
		double_ = 0x140A,			//GL_DOUBLE
		half_float = 0x140B,		//GL_HALF_FLOAT
	};

	enum class LIB04_OPENGL_API TextureInternalFormat :unsigned int {
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

	enum class LIB04_OPENGL_API TextureExternalFormat:unsigned int {
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

	//GLsync相关枚举
	enum class LIB04_OPENGL_API SyncCondition:unsigned int {
		SyncGPUCommandsComplete = 0x9117,    // GL_SYNC_GPU_COMMANDS_COMPLETE  当所有在调用 glFenceSync 之前提交到 OpenGL 命令流中的 GPU 命令全部执行完成时，同步对象被触发
	};

	enum class LIB04_OPENGL_API SyncFlags:unsigned int {
		none = 0,
		//在创建同步对象的同时，隐式执行一次 glFlush()，确保 glFenceSync 之前的所有命令被发送到 GPU（但不等待执行完成）
		//若不设置该标志，需显式调用 glFlush() 才能保证同步对象的触发条件有效（否则可能因命令未提交而导致同步对象永远不触发）
		SyncFlushCommandsBit = 0x00000001    // GL_SYNC_FLUSH_COMMANDS_BIT	
	};

	enum class LIB04_OPENGL_API SyncStatus:unsigned int {
		SyncStatusFailed =	0x911D,								//GL_WAIT_FAILED   等待同步对象失败
		SyncStatusTimeoutExpired = 0x911B,			//GL_TIMEOUT_EXPIRED	等待超时，同步对象未被触发
		SyncStatusConditionSatisfied	,							//GL_CONDITION_SATISFIED	同步对象在等待时间触发
		SyncStatusAlreadySignaled = 0x911A				// GL_ALREADY_SIGNALED		同步对象已经触发
	};

	enum class LIB04_OPENGL_API SyncObjectType:unsigned int {
		SyncTypeFence = 0x9116,    // GL_SYNC_FENCE
		SyncTypeSemaphore = 0x9115,    // GL_SYNC_SEMAPHORE
		SyncTypeEvent = 0x9114    // GL_SYNC_EVENT
	};

	//内存同步对象相关枚举
	enum class LIB04_OPENGL_API MemoryBarrierFlag:unsigned int {
		AllBarrierBits = 0xFFFFFFFF,    // GL_ALL_BARRIER_BITS
		VertexAttribArrayBarrierBit = 0x00000001,    // GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
		ElementArrayBarrierBit = 0x00000002,    // GL_ELEMENT_ARRAY_BARRIER_BIT
		UniformBarrierBit = 0x00000004,    // GL_UNIFORM_BARRIER_BIT
		TextureFetchBarrierBit = 0x00000008,    // GL_TEXTURE_FETCH_BARRIER_BIT
		ShaderImageAccessBarrierBit = 0x00000020,    // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
		CommandBarrierBit = 0x00000040,    // GL_COMMAND_BARRIER_BIT
		PixelBufferBarrierBit = 0x00000080,    // GL_PIXEL_BUFFER_BARRIER_BIT
		TextureUpdateBarrierBit = 0x00000100,    // GL_TEXTURE_UPDATE_BARRIER_BIT
		BufferUpdateBarrierBit = 0x00000200,    // GL_BUFFER_UPDATE_BARRIER_BIT
		FramebufferBarrierBit = 0x00000400,    // GL_FRAMEBUFFER_BARRIER_BIT
		TransformFeedbackBarrierBit = 0x00000800,    // GL_TRANSFORM_FEEDBACK_BARRIER_BIT
		AtomicCounterBarrierBit = 0x00001000,    // GL_ATOMIC_COUNTER_BARRIER_BIT
		AllMemoryBarrierBits = 0xFFFFFFFF    // GL_ALL_BARRIER_BITS	
	};

	struct LIB04_OPENGL_API GlSyncObject {
		void* ptr;		// GLsync对象指针
	};

	/// 顶点 细分控制 细分计算 几何 片段 计算一共6个着色器阶段
	enum class LIB04_OPENGL_API ShaderComponentType :unsigned int {
		//Uniform 组件限制
		XGL_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A,									//顶点着色器中uniform的最大组件数
		XGL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS = 0x8E7F,						//细分控制着色器的最大组件数
		XGL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS = 0x8E80,					//细分计算着色器的最大组件数
		XGL_MAX_GEOMETRY_UNIFORM_COMPONENTS = 0x8DDF,								//几何着色器中uniform的最大组件数
		XGL_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49,								//片段着色器中uniform的最大组件数
		XGL_MAX_COMPUTE_UNIFORM_BLOCKS = 0x91BB,											//计算着色器中uniform块的最大数量
		XGL_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A33,											//所有着色器阶段中uniform块的最大数量

		//纹理数量限制
		XGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C,									//顶点着色器中最多可声明的采样器数量
		XGL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS = 0x8E81,						//曲面细分控制着色器最大纹理单元
		XGL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS = 0x8E82,					//曲面细分计算着色器最大纹理单元
		XGL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29,								//几何着色器中最多可声明的采样器数量
		XGL_MAX_TEXTURE_IMAGE_UNITS = 0x8872,													//片段着色器中最多可声明的采样器数量
		XGL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0x91BC,									//计算着色器最大纹理单元
		XGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,								//所有着色器阶段总共可用的纹理单元数量

		//Uniform 块数量限制
		XGL_MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B,												//顶点着色器中uniform块的最大数量
		XGL_MAX_TESS_CONTROL_UNIFORM_BLOCKS = 0x8E85,									//细分控制着色器中uniform块的最大数量
		XGL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS = 0x8E86,							//细分计算着色器中uniform块的最大数量
		XGL_MAX_GEOMETRY_UNIFORM_BLOCKS = 0x8A2C,										//几何着色器中uniform块的最大数量
		XGL_MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D,										//片段着色器中uniform块的最大数量
		XGL_MAX_COMPUTE_UNIFORM_COMPONENTS = 0x8266,								//计算着色器中uniform的最大数量
		XGL_MAX_COMBINED_UNIFORM_COMPONENTS = 0x8B4A,								//所有着色器阶段中uniform的最大数量

		//着色器存储缓冲区限制
		XGL_MAX_VERTEX_SHADER_STORAGE_BLOCKS = 0x90D6,									//顶点着色器中最大的存储缓冲区数量
		XGL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS = 0x90D8,						//细分控制着色器中最大的存储缓冲区数量
		XGL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS = 0x90D9,				//细分计算着色器中最大的存储缓冲区数量
		XGL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS = 0x90D7,								//几何着色器中最大的存储缓冲区数量
		XGL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS = 0x90DA,								//片段着色器中最大的存储缓冲区数量
		XGL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS = 0x90DB,								//计算着色器中最大的存储缓冲区数量
		XGL_MAX_COMBINED_SHADER_STORAGE_BLOCKS = 0x90DC,								//所有着色器阶段中最大的存储缓冲区数量
	};
}
