#include "XTransformGizmoRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib04_opengl/XOpenGLFuntion.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <xsignal/XSignal.h>

#include "base/xbaserender/baseRender/XBaseRender.h"
#include "base/xbaserender/baseRender/XBaseRenderWindow.h"
#include "base/xbaserender/baseRender/XBaseRenderCamera.h"
#include <xrendernode/renderNode3d/XFullScreenQuadNode.h>
class XTransformGizmoRenderNode::Internal {
public:
	//操作柄由几部分几何组成的装配体 三个箭头、三个圆环,中心点是一个球体
	sptr<XArrowRenderNode> mArrowX;
	sptr<XArrowRenderNode> mArrowY;
	sptr<XArrowRenderNode> mArrowZ;
	sptr<XSphereRenderNode> mSphere;

	sptr< XTorusRenderNode> mRotateXY;			//绕着Z轴旋转
	sptr< XTorusRenderNode> mRotateYZ;				//绕着X轴旋转
	sptr< XTorusRenderNode> mRotateZX;				//绕着Y轴旋转

	sptr<XFullScreenQuadNode> mFullScreenQuadNode;

	wptr<XRenderNode> mBindRenderNode;
	sptr<XOpenGLEnable> mGlEnable;
	xsig::xconnector mConnector;
	~Internal() {
		mConnector.disconnect();
	}

	sptr<XOpenGLEnable> getGlEnable() {
		if (!mGlEnable) {
			mGlEnable = makeShareDbObject<XOpenGLEnable>();
		}
		return mGlEnable;
	}
};
XTransformGizmoRenderNode::XTransformGizmoRenderNode():mData(new Internal)
{

}

XTransformGizmoRenderNode::~XTransformGizmoRenderNode()
{
}

void XTransformGizmoRenderNode::Init()
{
	XGroupRenderNode3d::Init();
	float lineLen = 1;
	float radius = 0.2;
	XQ::Vec2f arrowSize(radius*2, radius*2);
	XQ::Vec2f lineSize(radius, 1);
	XQ_ATTR_ADD_INIT(AtteArrowSize, arrowSize);
	XQ_ATTR_ADD_INIT(AtteLineSize, lineSize);

	mData->mArrowZ = makeShareDbObject<XArrowRenderNode>();
	mData->mArrowZ->setSingleColor(XQ::Vec4f(0, 0, 1, 1));
	mData->mArrowZ->setPolygonMode(PolygonMode::all);
	mData->mArrowZ->setColorMode(ColorMode::SingleColor);
	mData->mArrowZ->setLineSize(lineSize[0],lineSize[1]);
	mData->mArrowZ->setArrowSize(arrowSize[0], arrowSize[1]);

	mData->mArrowX = makeShareDbObject<XArrowRenderNode>();
	mData->mArrowX->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
	mData->mArrowX->setPolygonMode(PolygonMode::all);
	mData->mArrowX->setColorMode(ColorMode::SingleColor);
	mData->mArrowX->setLineSize(lineSize[0], lineSize[1]);
	mData->mArrowX->setArrowSize(arrowSize[0], arrowSize[1]);
	mData->mArrowX->rotateY(90);

	mData->mArrowY = makeShareDbObject<XArrowRenderNode>();
	mData->mArrowY->setSingleColor(XQ::Vec4f(0, 1, 0, 1));
	mData->mArrowY->setPolygonMode(PolygonMode::all);
	mData->mArrowY->setColorMode(ColorMode::SingleColor);
	mData->mArrowY->setLineSize(lineSize[0], lineSize[1]);
	mData->mArrowY->setArrowSize(arrowSize[0], arrowSize[1]);
	mData->mArrowY->rotateX(270);

	mData->mSphere = makeShareDbObject<XSphereRenderNode>();
	mData->mSphere->scale(radius *1.5, radius * 1.5, radius * 1.5);

	mData->mRotateXY = makeShareDbObject<XTorusRenderNode>();
	mData->mRotateXY->setEndAngle(90);
	mData->mRotateXY->setMajorRadius(lineLen*0.5);
	mData->mRotateXY->setMinorRadius(radius);

	mData->mRotateYZ = makeShareDbObject<XTorusRenderNode>();
	mData->mRotateYZ->setEndAngle(90);
	mData->mRotateYZ->setMajorRadius(lineLen * 0.5);
	mData->mRotateYZ->setMinorRadius(radius);
	mData->mRotateYZ->rotateY(-90);

	mData->mRotateZX = makeShareDbObject<XTorusRenderNode>();
	mData->mRotateZX->setEndAngle(90);
	mData->mRotateZX->setMajorRadius(lineLen * 0.5);
	mData->mRotateZX->setMinorRadius(radius);
	mData->mRotateZX->rotateX(90);

	addChildRenderNode(mData->mArrowX);
	addChildRenderNode(mData->mArrowY);
	addChildRenderNode(mData->mArrowZ);
	addChildRenderNode(mData->mSphere);
	addChildRenderNode(mData->mRotateXY);
	addChildRenderNode(mData->mRotateYZ);
	addChildRenderNode(mData->mRotateZX);

	mData->mFullScreenQuadNode = makeShareDbObject<XFullScreenQuadNode>();
	mData->mFullScreenQuadNode->setColorMode(ColorMode::SingleColor);
	mData->mFullScreenQuadNode->setFarRect();


	mData->mConnector.connect(this,&XTransformGizmoRenderNode::sigItemDataChanged,[this](sptr<XDataAttribute> attr, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			if (attr == AtteLineSize || attr == AtteArrowSize) {
				auto arrowSize = AtteArrowSize->getValue();
				auto lineSize = AtteLineSize->getValue();
				float radius = lineSize[0];
				float lineLen = lineSize[1];

				mData->mArrowZ->setLineSize(lineSize[0], lineSize[1]);
				mData->mArrowZ->setArrowSize(arrowSize[0], arrowSize[1]);

				mData->mArrowX->setLineSize(lineSize[0], lineSize[1]);
				mData->mArrowX->setArrowSize(arrowSize[0], arrowSize[1]);

				mData->mArrowY->setLineSize(lineSize[0], lineSize[1]);
				mData->mArrowY->setArrowSize(arrowSize[0], arrowSize[1]);

				mData->mSphere->setScale(radius * 1.5, radius * 1.5, radius * 1.5);

				mData->mRotateXY->setMajorRadius(lineLen * 0.5);
				mData->mRotateXY->setMinorRadius(radius);

				mData->mRotateYZ->setMajorRadius(lineLen * 0.5);
				mData->mRotateYZ->setMinorRadius(radius);

				mData->mRotateZX->setMajorRadius(lineLen * 0.5);
				mData->mRotateZX->setMinorRadius(radius);
			}
		}
	});
}

void XTransformGizmoRenderNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	auto viewport = render->getConvertViewPort();
	auto w = viewport[2];
	auto h = viewport[3];
	//为了实现节点在屏幕中的固定大小，需要调整操作柄矩阵
	//计算局部坐标系下的一个单位，对应屏幕上的占据多少像素
	
	auto parent_mat = XQ::Matrix::convert(parentMatrix);
	auto object_mat =getTransform();
	auto total_mat = parent_mat * object_mat;
	auto transform_data = XQ::Matrix::transformDecomposition_TRS(parent_mat);
	Eigen::Vector3f pos = total_mat.translation();
	auto  z= pos.z();
	auto scale_h =render->getCamera()->scaleFactorH(z,w);		//相机宽度：屏幕宽度
	scale_h *= transform_data.sx;

	//auto scale_v = render->getCamera()->scaleFactorV(z, h);		//相机高度：屏幕高度
	//scale_v *= transform_data.sy;

	//已知屏幕固定像素 
	float fix_screen_size_h = 50;
	float fix_screen_size_w = 6;

	float line_real_len = fix_screen_size_h*scale_h;
	float line_real_radius = fix_screen_size_w*scale_h;

	XQ::Vec2f arrowSize(line_real_radius * 2, line_real_radius * 2);
	AtteLineSize->setValue(XQ::Vec2f(line_real_radius, line_real_len));
	AtteArrowSize->setValue(arrowSize);

	//为何实现节点不被阻挡，使用模板缓冲的方式 来做
	if (isNormal) {
		//1 启用模板测试，设置深度测试总是通过，但是不写入
		auto glEnable = mData->getGlEnable();
		glEnable->save();
		glEnable->enable(XOpenGLEnable::EnableType::STENCIL_TEST);
		XOpenGLFuntion::xglClearStencil(0);
		XOpenGLFuntion::xglClear((int)XOpenGL::BufferBits::stencil_buffer_bit);
		XOpenGLFuntion::xglStencilFunc(XOpenGL::DepthOrStencilCompFunType::XGL_ALWAYS,1,0xff);
		XOpenGLFuntion::xglStencilOp(XOpenGL::StencilBehavior::XGL_KEEP,XOpenGL::StencilBehavior::XGL_KEEP,XOpenGL::StencilBehavior::XGL_REPLACE);
		XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_ALWAYS);
		XOpenGLFuntion::xglDepthMask(false);
		XGroupRenderNode3d::draw(render,parentMatrix, isNormal);			//得到模板数值

		// 设置模板测试相关函数 ，关闭颜色输出
		//2 绘制全屏矩形，但是只有符合模板测试的片元才通过，只是将更新深度缓冲区为远平面
		glEnable->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
		XOpenGLFuntion::xglDepthMask(true);
		XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_ALWAYS);
		XOpenGLFuntion::xglStencilFunc(XOpenGL::DepthOrStencilCompFunType::XGL_EQUAL, 1, 0xff);
		XOpenGLFuntion::xglColorMask(false,false,false,false);
		if (!mData->mFullScreenQuadNode->getShaderManger()) {
			mData->mFullScreenQuadNode->setShaderManger(getShaderManger());
		}
		mData->mFullScreenQuadNode->draw(render,Eigen::Matrix4f::Identity(),true);

		glEnable->restore();
		
		// 恢复模板测试、深度测试 颜色掩码等为默认状态
		//3 重新绘制图元
		XOpenGLFuntion::xglColorMask(true, true, true, true);
		XOpenGLFuntion::xglDepthFunc(XOpenGL::DepthOrStencilCompFunType::XGL_LESS);
		XGroupRenderNode3d::draw(render,parentMatrix, isNormal);
	}
	else {
		XGroupRenderNode3d::draw(render,parentMatrix, isNormal);
	}
	
}

XQ::BoundBox XTransformGizmoRenderNode::getThisBoundBox(const Eigen::Matrix4f& m) const
{
	return {};
}

sptr<XRenderNode> XTransformGizmoRenderNode::getInteractObject(InteractObjectType type)
{
	switch (type)
	{
	case XTransformGizmoRenderNode::InteractObjectType::none:
		return sptr<XRenderNode>();
		break;
	case XTransformGizmoRenderNode::InteractObjectType::origin:
		return mData->mSphere;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::translate_x:
		return mData->mArrowX;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::translate_y:
		return mData->mArrowY;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::translate_z:
		return mData->mArrowZ;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::rotate_x:
		return mData->mRotateYZ;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::rotate_y:
	return mData->mRotateZX;
		break;
	case XTransformGizmoRenderNode::InteractObjectType::rotate_z:
	return mData->mRotateXY;
		break;
	default:
		break;
	}
}

XTransformGizmoRenderNode::InteractObjectType XTransformGizmoRenderNode::getInteractObjectType(sptr<XRenderNode> node)
{
	if (node == mData->mArrowX) {
		return InteractObjectType::translate_x;
	}
	else if (node == mData->mArrowY) {
		return InteractObjectType::translate_y;
	}
	else if(node == mData->mArrowZ)
	{
		return InteractObjectType::translate_z;
	}
	else if (node == mData->mRotateXY) {
		return InteractObjectType::rotate_z;
	}
	else if (node == mData->mRotateYZ) {
		return InteractObjectType::rotate_x;
	}
	else if (node == mData->mRotateZX) {
		return InteractObjectType::rotate_y;
	}
	else {
		return InteractObjectType::none;
	}
}

void XTransformGizmoRenderNode::bindRenderNode(sptr<XRenderNode> node)
{
	mData->mBindRenderNode = node;
	//初始位置姿态相同，缩放系数可能不同
	mData->mConnector.disconnect("SigMatrixChanged");
	if(!node)
		return;
	//同步node的位姿信息给this
	Eigen::Affine3f node_transform = node->getTransform();

	auto data = XQ::Matrix::transformDecomposition_TRS(node_transform);
	
	Eigen::Vector3f origin_pos = node_transform * Eigen::Vector3f(0, 0, 0);
	auto boundBox = node->getThisBoundBox(Eigen::Matrix4f::Identity());

	auto scale_x = boundBox.xmax - boundBox.xmin;
	auto scale_y = boundBox.ymax - boundBox.ymin;
	auto scale_z = boundBox.zmax - boundBox.zmin;
	//data.sx = scale_x;
	//data.sy = scale_y;
	//data.sz = scale_z;

	data.sx = 1;
	data.sy = 1;
	data.sz = 1;
	m_transform.matrix() = XQ::Matrix::computeMatrix(data);


	//绑定函数
	mData->mConnector.connect(this,&XTransformGizmoRenderNode::SigMatrixChanged,[node](const Eigen::Matrix4f& m){
		Eigen::Affine3f trans;
		trans.matrix() =m;

		Eigen::Vector3f origin_pos = trans*Eigen::Vector3f(0,0,0);

		//提取矩阵信息，更新物体
		auto gizmo_mat_data = XQ::Matrix::transformDecomposition_TRS(trans);
		auto node_transform = node->getTransform();

		auto node_mat_data = XQ::Matrix::transformDecomposition_TRS(node_transform);

		node_mat_data.tx = gizmo_mat_data.tx;
		node_mat_data.ty = gizmo_mat_data.ty;
		node_mat_data.tz = gizmo_mat_data.tz;

		node_mat_data.rx = gizmo_mat_data.rx;
		node_mat_data.ry = gizmo_mat_data.ry;
		node_mat_data.rz = gizmo_mat_data.rz;

		auto new_matrix =XQ::Matrix::computeMatrix(node_mat_data);
		trans.matrix() = new_matrix;
		node->setTransform(trans);

	},"SigMatrixChanged");
}

sptr<XRenderNode> XTransformGizmoRenderNode::getBindRenderNode()
{
	return mData->mBindRenderNode.lock();
}

void XTransformGizmoRenderNode::notifySigMatrixChanged()
{
	Eigen::Matrix4f m= getTransform().matrix();
	SigMatrixChanged(m);
}
