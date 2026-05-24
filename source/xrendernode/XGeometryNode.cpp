#include "XGeometryNode.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLType.h>
#include <lib04_opengl/XOpenGLFuntion.h>
#include <lib04_opengl/XOpenGLBuffer.h>

#include <Eigen/Eigen>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <lib01_shader/xshaderManger.h>

#include <glew/glew.h>

#include "datasource/xshapeSource.h"
#include "XGeometryNode.h"

#include "base/xbaserender/baseNode/XBaseRenderTexture.h"
#include "base/xbaserender/baseRender/XBaseDrawManger.h"
#include "base/xbaserender/baseRender/XBaseRenderCamera.h"

/**
 * @class XGeometryNode
 */
class XGeometryNode::Internal {
public:
	Internal(XGeometryNode* host) {}
	~Internal() {}

	std::mutex m_mutex;
};


XGeometryNode::XGeometryNode():mData(new Internal(this))
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

	adjustPos(render,shader,parentMatrix);

	auto faceIdx = m_polyMapper->getInput()->getFaceIndexArray();
	if (faceIdx) {
		State->AttrFaceNum->setValue(faceIdx->getNumOfTuple());
	}

	shader->use();
	
	State->bindBuffer();

	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	shader->setModelMatrix(matrix.data());
	shader->setObjectID(getID());
	auto preSelectColor = Attribute->AttrPreSelectColor->getValue();
	shader->setPreSelectColor(preSelectColor.r2(), preSelectColor.g2(), preSelectColor.b2(), preSelectColor.a2());
	shader->setColorMode((int)getColorMode());
	shader->setPolygonMode((int)getPolygonMode());
	shader->setBool("isNdc",Attribute->AttrIsNdc->getValue());
	shader->setUint("PickMode", (uint32_t)Attribute->AttrPickMode->getValue());
	shader->setUint("PrimitiveFaceNum", State->AttrFaceNum->getValue());
	shader->setBool("UseNoramlCamera", Attribute->AttrUseNormalCamera->getValue());
	auto singleColor = Attribute->AttrSingleColor->getValue();
	auto preselectColor = Attribute->AttrPreSelectColor->getValue();
	auto selectedColor = Attribute->AttrSelectedColor->getValue();

	shader->setSingleColor(singleColor.r2(), singleColor.g2(), singleColor.b2(), singleColor.a2());
	shader->setPreSelectColor(preselectColor.r2(), preselectColor.g2(), preselectColor.b2(), preselectColor.a2());
	shader->setSelectedColor(selectedColor.r2(), selectedColor.g2(), selectedColor.b2(), selectedColor.a2());

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
		auto w= fbo->getWidth();
		auto h = fbo->getHeight();
		fbo->bind();
		//绘制轮廓到边缘
		enable->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
		enable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
		//enable->enable(XOpenGLEnable::EnableType::STENCIL_TEST);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);
		XOpenGLFuntion::xglClearColor(XQ::Vec4u(0, 0, 0, 0), 1);
		XOpenGLFuntion::xglClearColor(XQ::Vec4f(0, 0, 0, 0), 0);
		XOpenGLFuntion::xglClearDepthStencil(1,0);
		auto rect = XOpenGLFuntion::xGetglViewport();

		draw(render, getShaderManger()->getShader3D(getDrawType()), parentMatrix);

		auto id_texture = fbo->getColorAttachment(1);
		/*auto ssss = id_texture->mapMultiSample2pbo(XOpenGLFramebufferObject::Attachment::Color)->map2cpu();
		auto c = ssss->heteroCast<uint32_t>(1);*/
		auto color_texture = fbo->getColorAttachment(0);

		auto ss = fbo->isComplete();
		fbo->release();

		enable->restore();

		//开始提取颜色
		drawManger->biltScreen();
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
		auto viewport = render->getConvertViewPort();
		auto rect2 = XOpenGLFuntion::xGetglViewport();
		getShaderManger()->getOutlineShader()->setVec2("viewPortOrigin", viewport[0], viewport[1]);
		
		draw(render, getShaderManger()->getOutlineShader(), parentMatrix);

		enable->restore();
	}
	
	Eigen::Matrix4f matrix = parentMatrix * m_transform.matrix();
	for (auto m : *renderNodes) {
		m->asDerived<XRenderNode>()->draw(render,matrix);
	}
}

void XGeometryNode::adjustPos(sptr<XBaseRender> render, std::shared_ptr<xshader>, const Eigen::Matrix4f& parentMatrix)
{
	//节点的位置类型 动态调整节点的矩阵
	//可能类型 在屏幕的固定位置 屏幕的固定大小
	auto parent_mat = XQ::Matrix::convert(parentMatrix);
	auto object_mat = getTransform();
	auto transform_object_data = XQ::Matrix::transformDecomposition_TRS(object_mat);
	auto total_mat = parent_mat * object_mat;
	auto viewport = render->getConvertViewPort();
	auto w = viewport[2];
	auto h = viewport[3];
	Eigen::Vector3f origin = total_mat.translation();
	origin = render->getCamera()->ComputeWorldToDisplay(origin);

	if (Attribute->AttrSizePolicy->AttrPositionType->getValue() == XRenderNodeOriginPositionType::fixed) {
		//固定位置大小
		auto pos = Attribute->AttrSizePolicy->AttrPositionPos->getValue();
		auto x = (float)pos[0] / (float)w;
		auto y = (float)pos[1] / (float)h;
		auto expactedPos = render->getCamera()->ComputeDisplayToWorld(Eigen::Vector3f(x, y, origin[2]));
		//
		auto new_pos = parent_mat.inverse() * expactedPos;
		transform_object_data.tx = new_pos[0];
		transform_object_data.ty = new_pos[1];
		transform_object_data.tz = new_pos[2];

		//缩放调整
		if (Attribute->AttrSizePolicy->AttrIsFixedSize->getValue()) {
			auto fixedPixel = Attribute->AttrSizePolicy->AttrFixedPixel->getValue();
			//需要获取原始物体的大小
			auto boundBox = m_polyMapper->getInput()->getBoundBox(Eigen::Affine3f::Identity());
			auto len_x = std::max(abs(boundBox.xmax), abs(boundBox.xmin));

			auto transform_data = XQ::Matrix::transformDecomposition_TRS(parent_mat);
			auto  z = origin.z();
			auto scale_x = render->getCamera()->scaleFactorH(z, w);		//相机宽度：屏幕宽度
			scale_x *= transform_data.sx;

			float line_real_len = fixedPixel[0] * scale_x;
			float object_scale_x = line_real_len / len_x;

			transform_object_data.sx = object_scale_x;
			transform_object_data.sy = object_scale_x;
			transform_object_data.sz = object_scale_x;
		}

		auto mat = XQ::Matrix::computeMatrix(transform_object_data);
		object_mat.matrix() = mat;
		this->setTransform(object_mat);
	}
	else if(Attribute->AttrSizePolicy->AttrIsFixedSize->getValue()) {
		//是否固定大小
		{
			auto fixedPixel = Attribute->AttrSizePolicy->AttrFixedPixel->getValue();
			//需要获取原始物体的大小
			auto boundBox = m_polyMapper->getInput()->getBoundBox(Eigen::Affine3f::Identity());
			auto len_x = std::max(abs(boundBox.xmax), abs(boundBox.xmin));

			auto transform_data = XQ::Matrix::transformDecomposition_TRS(parent_mat);
			auto  z = origin.z();
			auto scale_x = render->getCamera()->scaleFactorH(z, w);		//相机宽度：屏幕宽度
			scale_x *= transform_data.sx;

			float line_real_len = fixedPixel[0] * scale_x;
			float object_scale_x = line_real_len / len_x;

			transform_object_data.sx = object_scale_x;
			transform_object_data.sy = object_scale_x;
			transform_object_data.sz = object_scale_x;
		}

		auto mat = XQ::Matrix::computeMatrix(transform_object_data);
		object_mat.matrix() = mat;
		this->setTransform(object_mat);
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
	std::lock_guard<std::mutex> lock(mData->m_mutex);
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
	std::lock_guard<std::mutex> lock(mData->m_mutex);
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

void XGeometryNode::setSelectedColor(XQ::Vec4f color)
{
	Attribute->AttrSelectedColor->setValue(XQ::XColor::from_normalcolor(color));
}

XQ::Vec4f XGeometryNode::getSelectedColor() const
{
	auto tmp = Attribute->AttrSelectedColor->getValue();
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
	if (Attribute->AttrIsValidBoundBox->getValue()) {
		Eigen::Affine3f t;
		t.matrix() = m * m_transform.matrix();
		return m_polyMapper->getInput()->getBoundBox(t);
	}
	else {
		return {};
	}
	
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

sptr<XShapeSource> XGeometryNode::getInput()
{
	return getOrCreateMapper()->getInput();
}

void XGeometryNode::Init()
{
	XRenderNode3D::Init();
	XQ_XDATA_ADD(Attribute);
	XQ_XDATA_ADD_EXT(State,false);
}