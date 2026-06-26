#include "XPolyLineRenderNode.h"
#include <xrendernode/datasource/xpolygonLineSource.h>
#include <xrendernode/mapper/XPolyDataAjacencyMapper.h>
#include <base/xbaserender/baseRender/XBaseRender.h>
#include <base/xbaserender/baseRender/XBaseRenderCamera.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLFuntion.h>

class XPolyLineRenderNode::Internal {
	public:
		sptr<XPolygonLineSource> source;
		Internal(){
			source = makeShareDbObject<XPolygonLineSource>();
		}
};

XPolyLineRenderNode::XPolyLineRenderNode():mData(new Internal)
{
	
}

XPolyLineRenderNode::~XPolyLineRenderNode()
{

}

void XPolyLineRenderNode::Init()
{
	XGeometryNode::Init();
	XQ_ATTR_ADD_INIT(AttrLineWidth,3);
	XQ_ATTR_ADD_INIT(AttrPlaneNormal, XQ::Vec3f(0, 0, 1));
	XQ_ATTR_ADD_INIT(AttrOffset, 0);
	XQ_ATTR_ADD_INIT(AttrPenStyle,PenStyle::Solid);
	Attribute->AttrPrimitveType->setValue(PrimitveType::line_strip_adjacency);
	setPolygonMode(PolygonMode::line);
	this->setInput(mData->source);
}

sptr<XFloatArray> XPolyLineRenderNode::getPolygonLineCoords() const
{
	return mData->source->getPolygonLineCoords();
}

sptr<XPolyDataMapper> XPolyLineRenderNode::getOrCreateMapper()
{
	if (getPolyDataMapper()) {
		return getPolyDataMapper();
	}
	else {
		setPolyDataMapper(makeShareDbObject<XPolyDataAjacencyMapper>());
		return getPolyDataMapper();
	}
}

void XPolyLineRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto shader = getShaderManger()->getShader3D(PrimitveType::line_strip_adjacency);
	shader->use();
	auto normal = AttrPlaneNormal->getValue();
	shader->setVec3("u_planeNormal", normal[0],normal[1],normal[2]);
	shader->setBool("u_isComputeLineLentgh", false);
	shader->setInt("u_penStyle", (int)AttrPenStyle->getValue());

	auto expacted_len = AttrLineWidth->getValue();

	shader->setUint("u_lineWidth", expacted_len);
	shader->setFloat("u_offset", AttrOffset->getValue());
	{
		auto parent_mat = XQ::Matrix::convert(parentMatrix);
		auto transform_data = XQ::Matrix::transformDecomposition_TRS(parent_mat);
		auto viewport = render->getConvertViewPort();
		auto w = viewport[2];
		auto h = viewport[3];

		auto camera = render->getCamera();
		auto scale_x = camera->scaleFactorH(0, w);		//相机宽度：屏幕宽度
		
		float line_real_len = expacted_len * scale_x;		//得到相机坐标系下的宽度

		shader->setUint("u_lineWidth", line_real_len);
	}

	auto old = XOpenGLFuntion::xglDrawBuffers({ XOpenGL::XGL_COLOR_ATTACHMENT0 });
	XGeometryNode::draw(render, shader, parentMatrix);
	XOpenGLFuntion::xglDrawBuffers({old });
}

