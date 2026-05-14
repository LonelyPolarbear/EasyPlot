#include "XGeometryNode.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLFuntion.h>

#include <Eigen/Eigen>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <lib01_shader/xshaderManger.h>

#include <glew/glew.h>

#include "datasource/xshapeSource.h"
#include "XGeometryNode.h"

#include "base/xbaserender/baseNode/XBaseRenderTexture.h"
#include "base/xbaserender/baseRender/XBaseDrawManger.h"



class XGeometryNode::Internal {
public:
	std::mutex m_mutex;
};
XGeometryNode::XGeometryNode():d(new Internal)
{
	Modified();
}

XGeometryNode::~XGeometryNode()
{
}

void XGeometryNode::draw(sptr<XBaseRender>  render, std::shared_ptr<xshader> shader, const Eigen::Matrix4f& parentMatrix)
{
	if(!isVisible())
		return;

	if(!shader)
		return;

	shader->use();

	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	shader->setModelMatrix(matrix.data());
	shader->setObjectID(getID());
	auto preSelectColor = Attribute->AttrPreSelectColor->getValue();
	shader->setPreSelectColor(preSelectColor.r2(), preSelectColor.g2(), preSelectColor.b2(), preSelectColor.a2());
	shader->setColorMode((int)getColorMode());
	shader->setPolygonMode((int)getPolygonMode());
	shader->setBool("isNdc",Attribute->AttrIsNdc->getValue());
	shader->setUint("PickMode",(uint32_t)Attribute->AttrPickMode->getValue());
	auto singleColor = Attribute->AttrSingleColor->getValue();
	shader->setSingleColor(singleColor.r2(), singleColor.g2(), singleColor.b2(), singleColor.a2());
	auto textureNum = getRenderTextureNum();
	for (int i = 0; i < textureNum; i++) {
		auto tex = getRenderTexture(i);
		tex->getTexture()->bindUnit(3+i);
	}
	m_polyMapper->draw(shader,getPolygonMode(),getDrawType());

}

void XGeometryNode::draw(sptr<XBaseRender>  render, const Eigen::Matrix4f& parentMatrix)
{
	bool isDarwOutline = Attribute->AttrDrawOutline->getValue();
	draw(render, getShaderManger()->getShader3D(getDrawType()), parentMatrix);

	/*
	* 实际使用发现多重采样深度模板纹理附件的诸多问题，无法片元着色器中采样，采用纹理视图仍然哦有问题
	* 因此先bilt到非多重采样纹理附件中
	*/
	if (isDarwOutline) {
		auto drawManger =render->getDrawManger();

		//希望绘制出轮廓线
		auto enable = makeShareDbObject<XOpenGLEnable>();
		enable->save();

		auto fbo = drawManger->getOverlayFbo(0)->asDerived<XOpenGLFramebufferObject>();
		fbo->bind();
		//绘制轮廓到边缘
		enable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
		enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
		enable->enable(XOpenGLEnable::EnableType::STENCIL_TEST);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);
		XOpenGLFuntion::xglClearColor(XQ::Vec4u(0, 0, 0, 0), 1);
		XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 0), 0);
		XOpenGLFuntion::xglClearDepthStencil(1,0);

		draw(render, getShaderManger()->getShader3D(getDrawType()), parentMatrix);

		auto id_texture = fbo->getColorAttachment(1);
		auto color_texture = fbo->getColorAttachment(0);

		auto ss = fbo->isComplete();
		fbo->release();

		enable->restore();

		//开始提取颜色
		drawManger->bilt();
		auto biltFbo = drawManger->getBiltFbo()->asDerived<XOpenGLFramebufferObject>();
		auto depth_stencil_tex = biltFbo->getDepthStencilAttachment();			//记录的深度模板附件

		auto screen_fbo = drawManger->getScreenFbo()->asDerived<XOpenGLFramebufferObject>();
		screen_fbo->bind();
		enable->save();
		enable->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
		enable->disable(XOpenGLEnable::EnableType::STENCIL_TEST);


		getShaderManger()->getOutlineShader()->use();
		id_texture->bindUnit(3);
		color_texture->bindUnit(4);
		depth_stencil_tex->bindUnit(5);
		
		draw(render, getShaderManger()->getOutlineShader(), parentMatrix);

		enable->restore();
	}
	
	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	for (auto m : *renderNodes) {
		m->asDerived<XRenderNode>()->draw(render,matrix);
	}
}

const float* XGeometryNode::getMatrix() const
{
	return m_transform.matrix().data();
}

void XGeometryNode::setVisible(bool visible) {
	XRenderNode::setVisible(visible);
}

bool XGeometryNode::isVisible() {
	return XRenderNode::isVisible();
}



void XGeometryNode::setPolygonMode(PolygonMode mode)
{
	Attribute->AttrPolygonMode->setValue(mode);
}

PolygonMode XGeometryNode::getPolygonMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return Attribute->AttrPolygonMode->getValue();
}

PrimitveType XGeometryNode::getDrawType() const
{
	return Attribute->AttrPrimitveType->getValue();
}

void XGeometryNode::setDrawType(PrimitveType type)
{
	Attribute->AttrPrimitveType->setValue(type);
}

void XGeometryNode::setColorMode(ColorMode mode)
{
	Attribute->AttrColorMode->setValue(mode);
}

ColorMode XGeometryNode::getColorMode() const
{
	std::lock_guard<std::mutex> lock(d->m_mutex);
	return Attribute->AttrColorMode->getValue();
}

void XGeometryNode::setSingleColor(XQ::Vec4f color)
{
	Attribute->AttrSingleColor->setValue(XQ::XColor::from_normalcolor(color) );
}

XQ::Vec4f XGeometryNode::getSingleColor() const
{
	auto tmp = Attribute->AttrSingleColor->getValue();
	return XQ::Vec4f(tmp.r2(), tmp.g2(), tmp.b2(), tmp.a());
}

void XGeometryNode::setPreSelectColor(XQ::Vec4f color) {
	Attribute->AttrPreSelectColor->setValue(XQ::XColor::from_normalcolor(color));
}

XQ::Vec4f XGeometryNode::getPreSelectColor() const {
	auto tmp = Attribute->AttrPreSelectColor->getValue();
	return XQ::Vec4f(tmp.r2(), tmp.g2(), tmp.b2(), tmp.a());
}

XQ::BoundBox XGeometryNode::getThisBoundBox(const Eigen::Matrix4f& m) const
{
	Eigen::Affine3f t;
	t.matrix() = m*m_transform.matrix();
	return m_polyMapper->getInput()->getBoundBox(t);
}

void XGeometryNode::setPolyDataMapper(sptr<XPolyDataMapper> mapper)
{
	m_polyMapper = mapper;
}

sptr<XPolyDataMapper> XGeometryNode::getPolyDataMapper() const
{
	return m_polyMapper;
}

sptr<XPolyDataMapper> XGeometryNode::getOrCreateMapper()
{
	if (getPolyDataMapper()) {
		return getPolyDataMapper();
	}
	else {
		setPolyDataMapper(makeShareDbObject<XPolyDataMapper>());
		return getPolyDataMapper();
	}
}

void XGeometryNode::setInput(sptr<XShapeSource> input)
{
	getOrCreateMapper()->setInput(input);
}

void XGeometryNode::Init()
{
	XRenderNode3D::Init();
	XQ_XDATA_ADD(Attribute);
	//Attribute = makeShareDbObject<XRenderNode3DAttribute>();
}