#include "XScreenTextRenderNode.h"
#include "xrendernode/mapper/XTextMapper.h"
#include "xrendernode/datasource/xtextSource.h"
#include "base/xbaserender/baseRender/XBaseRender.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"
#include "base/xbaserender/baseRender/XBaseRenderFontManger.h"
#include "base/xbaserender/baseRender/XBaseRenderScreenCamera.h"
#include "xsignal/XSignal.h"
#include "xcodecvt/XCodeCvt.h"
#include "lib01_shader/xshaderManger.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"


class XScreenTextRenderNode::Internal {

};

XScreenTextRenderNode::XScreenTextRenderNode():mData(new Internal)
{

}

XScreenTextRenderNode::~XScreenTextRenderNode()
{

}

void XScreenTextRenderNode::Init()
{
	XTextRenderNode::Init();

	Attribute->AttrSizePolicy->AttrIsFixedSize->setValue(true);																					//¹Ì¶¨³ß´ç
	Attribute->AttrSizePolicy->AttrPositionType->setValue(XRenderNodeOriginPositionType::fixed);						//¹Ì¶¨Î»ÖÃ
}

void XScreenTextRenderNode::setOrien(XRenderNodeOriginPositionOrien orien)
{
	Attribute->AttrSizePolicy->AttrPositionOrien->setValue(orien);		
}

void XScreenTextRenderNode::setPos(XQ::Vec2i pos)
{
	Attribute->AttrSizePolicy->AttrPositionPos->setValue(pos);
}
