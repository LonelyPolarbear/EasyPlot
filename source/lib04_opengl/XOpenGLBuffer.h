#pragma once
#include "XOpenglApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataArray.h>
#include "XOpenGLType.h"

enum class LIB04_OPENGL_API XOpenGLValueType {
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

class LIB04_OPENGL_API XOpenGLBuffer:public XDataBaseObject {
public:
	enum Access {
		ReadOnly= 0x88B8,				//GL_READ_ONLY
		WriteOnly= 0x88B9,				//GL_WRITE_ONLY
		ReadWrite = 0x88BA,				//GL_READ_WRITE
	};

	enum Type {
		VertexBuffer= 0x8892,							//GL_ARRAY_BUFFER
		IndexBuffer= 0x8893,							//GL_ELEMENT_ARRAY_BUFFER
		PixelPackBuffer= 0x88EB,						//GL_PIXEL_PACK_BUFFER
		PixelUnpackBuffer= 0x88EC,					//GL_PIXEL_UNPACK_BUFFER
		ShaderStorageBuffer= 0x90D2,				//GL_SHADER_STORAGE_BUFFER
		UniformBuffer= 0x8A11,						//GL_UNIFORM_BUFFER
		TransformFeedbackBuffer= 0x8C8E,		//GL_TRANSFORM_FEEDBACK_BUFFER
		
	};

	enum UsagePattern
	{
		StreamDraw = 0x88E0,				// GL_STREAM_DRAW
		StreamRead = 0x88E1,				// GL_STREAM_READ
		StreamCopy = 0x88E2,				// GL_STREAM_COPY
		StaticDraw = 0x88E4,				// GL_STATIC_DRAW
		StaticRead = 0x88E5,					// GL_STATIC_READ
		StaticCopy = 0x88E6,					// GL_STATIC_COPY
		DynamicDraw = 0x88E8,			// GL_DYNAMIC_DRAW
		DynamicRead = 0x88E9,			// GL_DYNAMIC_READ
		DynamicCopy = 0x88EA				// GL_DYNAMIC_COPY
	};

	enum RangeAccessFlags
	{
		RangeRead = 0x0001, // GL_MAP_READ_BIT
		RangeWrite = 0x0002, // GL_MAP_WRITE_BIT
		RangeInvalidate = 0x0004, // GL_MAP_INVALIDATE_RANGE_BIT
		RangeInvalidateBuffer = 0x0008, // GL_MAP_INVALIDATE_BUFFER_BIT
		RangeFlushExplicit = 0x0010, // GL_MAP_FLUSH_EXPLICIT_BIT
		RangeUnsynchronized = 0x0020  // GL_MAP_UNSYNCHRONIZED_BIT
	};

protected:
    XOpenGLBuffer();
    ~XOpenGLBuffer();
public:
	void setBufferType(XOpenGLBuffer::Type type);

	bool create();

	template<typename T>
	bool allocate(const T* data, int count) {
		return allocate(reinterpret_cast<const void*>(data), count * sizeof(T));
	}

	template<typename T>
	bool write(int offset, const T* data, int count) {
		return write(offset * sizeof(T), reinterpret_cast<const void*>(data), count * sizeof(T));
	}

	template<typename T>
	bool read(int offset, T*& data, int count) {
		return read(offset * sizeof(T), reinterpret_cast<void*>(data), count * sizeof(T));
	}

	bool allocate(int count);

	uint32_t bufferSize() const;

	void* map(XOpenGLBuffer::Access access);

	sptr<XUCharArray> map2cpu();

	bool unmap();

	void* mapRange(int offset, int count, XOpenGLBuffer::RangeAccessFlags access);

	bool bind();

	void release();

	void destroy();

	bool setBufferBindIdx(unsigned int idx);
	bool setFeedbackBufferBindIdx(unsigned int idx,sptr<XOpenGLBuffer> vbo);

	unsigned int bufferId();

	//XOpenGLBuffer::Type getType() const;

	void setUsagePattern(XOpenGLBuffer::UsagePattern value);

	XOpenGLBuffer::UsagePattern getUsagePattern() const;

	XOpenGL::DataBufferBindingType getBufferBindType() const;
protected:
	bool allocate(const void* data, int count);

	bool write(int offset, const void* data, int count);

	bool read(int offset, void*& data, int count);
	
protected:
	class Internal;
	std::unique_ptr<Internal> d;
};