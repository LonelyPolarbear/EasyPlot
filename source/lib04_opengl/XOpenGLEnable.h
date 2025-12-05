#pragma once
#include "XOpenglApi.h"
#include <dataBase/dataobject.h>
#include <glew/glew.h>

class LIB04_OPENGL_API XOpenGLEnable :public DataBaseObject {
public:
enum class EnableType:unsigned int {
	DEPTH_TEST = 0x0B71,							//GL_DEPTH_TEST
	MULTISAMPLE = 0x809D,						//GL_MULTISAMPLE
	SCISSOR_TEST = 0x0C11,						//GL_SCISSOR_TEST
	BLEND = 0x0BE2,									//GL_BLEND
	CULL_FACE = 0x0B44,							//GL_CULL_FACE
	PROGRAM_POINT_SIZE = 0x8642,
	STENCIL_TEST = 0x0B90,						//GL_STENCIL_TEST
};
protected:
	XOpenGLEnable();
	virtual ~XOpenGLEnable();
public:
	void enable(EnableType type);

	void disable(EnableType type);

	bool isEnabled(EnableType type);

	void restore();
protected:
	class Intertal;
private:
	std::unique_ptr<Intertal> d;
};