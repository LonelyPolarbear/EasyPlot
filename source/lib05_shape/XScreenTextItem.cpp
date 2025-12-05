#include "XScreenTextItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib01_shader/xshaderManger.h>
#include <lib08_freetype/xfreetype.h>
#include <Eigen/Eigen>
#include "XTextItem.h"


XScreenTextItem::XScreenTextItem(std::shared_ptr<XGraphicsItem> parent):XTextItem(parent)
{
	this->setPositionType(XGL::PositionType::sceneScreen_complete);
}

XScreenTextItem::~XScreenTextItem()
{
}

void XScreenTextItem::setTextSceneScreenPos(int x, int y) {

}
