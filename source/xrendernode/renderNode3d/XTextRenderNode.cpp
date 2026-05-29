#include "XTextRenderNode.h"
#include "xrendernode/mapper/XTextMapper.h"
#include "xrendernode/datasource/xtextSource.h"
#include "base/xbaserender/baseRender/XBaseRender.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"
#include "base/xbaserender/baseRender/XBaseRenderFontManger.h"
#include "xsignal/XSignal.h"
#include "xcodecvt/XCodeCvt.h"
#include "lib01_shader/xshaderManger.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"

class XTextRenderNode::Internal {
public:
	sptr<XTextSource> mTextSource;
	xsig::xconnector mCon;
	~Internal() {
		mCon.disconnect();
	}
};

XTextRenderNode::XTextRenderNode():mData(new Internal)
{

}

XTextRenderNode::~XTextRenderNode()
{

}

sptr<XPolyDataMapper> XTextRenderNode::getOrCreateMapper()
{
	if (getPolyDataMapper()) {
		return getPolyDataMapper();
	}
	else {
		setPolyDataMapper(makeShareDbObject<XTextMapper>());
		return getPolyDataMapper();
	}
}

void XTextRenderNode::createSource()
{
	mData->mTextSource = makeShareDbObject<XTextSource>();

	this->setInput(mData->mTextSource);

	mData->mTextSource->Modified();
}

void XTextRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto enable = makeShareDbObject<XOpenGLEnable>();
	enable->save();

	enable->enable(XOpenGLEnable::EnableType::BLEND);
	XOpenGLFuntion::xglBlendFunc(XOpenGL::BlendFuncFactor::XGL_SRC_ALPHA, XOpenGL::BlendFuncFactor::XGL_ONE_MINUS_SRC_ALPHA);
	auto fontManger =render->getRenderWindow()->getFontManger();
	auto texture = fontManger->getFontTexture()->asDerived<XOpenGLTexture>();
	
	auto textShader =getShaderManger()->getTextShader();
	textShader->use();
	texture->bindUnit(10);
	XGeometryNode::draw(render,textShader,parentMatrix);

	enable->restore();
}

void XTextRenderNode::Init()
{
	XGeometryNode::Init();
	XQ_ATTR_ADD_INIT(AttrText,std::string(""));
	Attribute->AttrSingleColor->setValue(XQ::XColor(255,0,0,255));
	createSource();

	mData->mCon.connect(AttrText,&XAttr_String::sigAttrChanged,[this](sptr<XDataAttribute> attr, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			auto value =attr->asDerived<XAttr_String>()->getValue();
			auto wstr =XCodeCvt::string_to_wstring(value);
			setText(wstr);
		}
	});
}

void XTextRenderNode::setText(const std::wstring& text)
{
	mData->mTextSource->setText(text);
}

void XTextRenderNode::setFontSize(double fontSize)
{
	mData->mTextSource->setFontSize(fontSize);
}

void XTextRenderNode::setFixedWidth(double width)
{
	mData->mTextSource->setFixedWidth(width);
}

void XTextRenderNode::setIsFixed(bool isFixed)
{
	mData->mTextSource->setIsFixed(isFixed);
}

std::wstring XTextRenderNode::getText() const
{
	return mData->mTextSource->getText();
}

double XTextRenderNode::getFontSize() const
{
	return mData->mTextSource->getFontSize();
}

double XTextRenderNode::getFixedWidth() const
{
	return mData->mTextSource->getFixedWidth();
}

bool XTextRenderNode::isFixedWidth() const
{
	return mData->mTextSource->isFixedWidth();
}