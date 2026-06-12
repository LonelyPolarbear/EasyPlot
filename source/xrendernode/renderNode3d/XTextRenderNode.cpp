#include "XTextRenderNode.h"
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

void XTextRenderNode::adjustScreenCameraPos(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix)
{
	auto view_port =render->getConvertViewPort();
	auto w = view_port[2];
	auto h = view_port[3];
	auto camera = render->getSceenCamera();
	auto neraPlaneFrame = camera->getNearPlaneFrame(render);
	auto virtualScreenFrame = camera->getVirtualScreenFrame(render);

	auto parent_mat = XQ::Matrix::convert(parentMatrix);
	auto object_mat = getTransform();
	auto transform_object_data = XQ::Matrix::transformDecomposition_TRS(object_mat);
	
	this->getInput()->getInstancedArray();		//获取第一个实例，获取示例

	Eigen::Affine3f virtualScreenAffine, neraPlaneAffine;
	virtualScreenAffine.matrix() = virtualScreenFrame;
	neraPlaneAffine.matrix() = neraPlaneFrame;
	if (Attribute->AttrSizePolicy->AttrPositionType->getValue() == XRenderNodeOriginPositionType::fixed) {
		//屏幕固定位置
		auto pos = Attribute->AttrSizePolicy->AttrPositionPos->getValue();
		

		//将视口固定位置，转换位virtualScreenAffine的位置
		auto orien = Attribute->AttrSizePolicy->AttrPositionOrien->getValue();
		switch (orien)
		{
		case XRenderNodeOriginPositionOrien::left_top:
		{
			pos[0] = -0.5*w +pos[0];
			pos[1] = 0.5*h -pos[1];
		}
			break;
		case XRenderNodeOriginPositionOrien::left_bottom:
		{
			pos[0] = -0.5 * w + pos[0];
			pos[1] = -0.5 * h + pos[1];
		}
			break;
		case XRenderNodeOriginPositionOrien::right_top:
		{
			pos[0] = 0.5 * w - pos[0];
			pos[1] = 0.5 * h - pos[1];
		}
			break;
		case XRenderNodeOriginPositionOrien::right_bottom:
		{
			pos[0] = 0.5 * w - pos[0];
			pos[1] = -0.5 * h + pos[1];
		}
			break;
		case XRenderNodeOriginPositionOrien::center:
			pos = pos;
			break;
		default:
			break;
		}

		Eigen::Vector3f new_pos = parent_mat.inverse() * virtualScreenAffine.inverse() * Eigen::Vector3f(pos[0], pos[1], 0);
		auto ppp = virtualScreenAffine * parent_mat * new_pos;
		auto sss = neraPlaneAffine * ppp;
		transform_object_data.tx = new_pos[0];
		transform_object_data.ty = new_pos[1];
		transform_object_data.tz = new_pos[2];
	}
	if (Attribute->AttrSizePolicy->AttrIsFixedSize->getValue()) {
		//是否固定大小
		auto fixedPixel = Attribute->AttrSizePolicy->AttrFixedPixel->getValue();
		//需要获取原始物体的大小
		auto boundBox = m_polyMapper->getInput()->getBoundBox(Eigen::Affine3f::Identity());
		auto len_x = std::max(abs(boundBox.xmax), abs(boundBox.xmin));
		auto len_y = std::max(abs(boundBox.ymax), abs(boundBox.ymin));

		auto transform_data = XQ::Matrix::transformDecomposition_TRS(virtualScreenAffine * parent_mat);

		auto scale_x = transform_data.sx;
		auto scale_y = transform_data.sy;

		float line_real_len = /*fixedPixel[0]*/len_x / scale_x;
		float line_real_len_y = /*fixedPixel[1]*/len_y / scale_y;
		float object_scale_x = line_real_len / len_x;
		float object_scale_y = line_real_len_y / len_y;

		transform_object_data.sx = object_scale_x;
		transform_object_data.sy = object_scale_y;

		transform_object_data.sx = 1. / scale_x;
		transform_object_data.sy = 1. / scale_y;

		transform_object_data.sz = 1;
	}

	//最后更新物体矩阵
	{
		auto mat = XQ::Matrix::computeMatrix(transform_object_data);
		object_mat.matrix() = mat;
		this->setTransform(object_mat);
	}
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
	textShader->setFloat("minDerivate", AttrMinDerivate->getValue());
	textShader->setFloat("maxDerivate", AttrMaxDerivate->getValue());
	XGeometryNode::draw(render,textShader,parentMatrix);

	enable->restore();
}

void XTextRenderNode::Init()
{
	XGeometryNode::Init();
	XQ_ATTR_ADD(AttrText);
	XQ_ATTR_ADD(AttrFontSize);
	XQ_ATTR_ADD(AttrIsFixedWidth);
	XQ_ATTR_ADD(AttrFixedWidth);
	XQ_ATTR_ADD(AttrVerAlign);
	XQ_ATTR_ADD(AttrHorAlign);
	XQ_ATTR_ADD_INIT(AttrMinDerivate, 0);
	XQ_ATTR_ADD_INIT(AttrMaxDerivate, 1);


	Attribute->AttrSingleColor->setValue(XQ::XColor(255,0,0,255));
	createSource();

	//信号绑定
	mData->mCon.connect(AttrText,&XAttr_String::sigAttrChanged,[this](sptr<XDataAttribute> attr, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			auto value =attr->asDerived<XAttr_String>()->getValue();
			auto wstr =XCodeCvt::string_to_wstring(value);
			mData->mTextSource->setText(wstr);
		}
	});

	mData->mCon.connect(AttrFontSize, &XAttr_Int::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			auto value = attr->asDerived<XAttr_Int>()->getValue();
			mData->mTextSource->setFontSize(value);
		}
		});

	mData->mCon.connect(AttrIsFixedWidth, &XAttr_Bool::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			auto value = attr->asDerived<XAttr_Bool>()->getValue();
			mData->mTextSource->setIsFixed(value);
		}
		});

	mData->mCon.connect(AttrFixedWidth, &XAttr_Double::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			auto value = attr->asDerived<XAttr_Double>()->getValue();
			mData->mTextSource->setFixedWidth(value);
		}
		});

	mData->mCon.connect(AttrVerAlign, &XAttr_Enum<VAlign>::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			auto value = attr->asDerived<XAttr_Enum<VAlign>>()->getValue();
			mData->mTextSource->setTextVerAlignment(value);
		}
		});

	mData->mCon.connect(AttrHorAlign, &XAttr_Enum<HAlign>::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
		if (type == XDataChangeType::ItemDataModified) {
			auto value = attr->asDerived<XAttr_Enum<HAlign>>()->getValue();
			mData->mTextSource->setTextHorAlignment(value);
		}
		});

	AttrText->setValue("");
	AttrFontSize->setValue(10);
	AttrIsFixedWidth->setValue(false);
	AttrFixedWidth->setValue(10);
	AttrVerAlign->setValue(VAlign::Top);
	AttrHorAlign->setValue(HAlign::Left);
}

void XTextRenderNode::setTextColor(const XQ::XColor& c)
{
	Attribute->AttrSingleColor->setValue(c);
}

void XTextRenderNode::setText(const std::string& text)
{
	AttrText->setValue(text);
}

void XTextRenderNode::setFontSize(double fontSize)
{
	AttrFontSize->setValue(fontSize);
}

void XTextRenderNode::setFixedWidth(double width)
{
	AttrFixedWidth->setValue(width);
}

void XTextRenderNode::setIsFixed(bool isFixed)
{
	AttrIsFixedWidth->setValue(isFixed);
}

void XTextRenderNode::setTextHorAlignment(HAlign alignment)
{
	AttrHorAlign->setValue(alignment);
}

void XTextRenderNode::setTextVerAlignment(VAlign alignment)
{
	AttrVerAlign->setValue(alignment);
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

HAlign XTextRenderNode::getTextHorAlignment() const
{
	return mData->mTextSource->getTextHorAlignment();
}

VAlign XTextRenderNode::getTextVerAlignment() const
{
	return mData->mTextSource->getTextVerAlignment();
}
