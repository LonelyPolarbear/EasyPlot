#include "XScreenTextItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshaderManger.h>
#include <lib08_freetype/xfreetype.h>
#include <Eigen/Eigen>
#include "XTextItem.h"


XScreenTextItem::XScreenTextItem():XTextItem()
{
}

XScreenTextItem::~XScreenTextItem()
{
}

void XScreenTextItem::setTextSceneScreenPos(int x, int y) {
	m_scene_screenPos = myUtilty::Vec2i(x,y);
}

myUtilty::Vec2i XScreenTextItem::getTextSceneScreenPos() const
{
	return m_scene_screenPos;
}
