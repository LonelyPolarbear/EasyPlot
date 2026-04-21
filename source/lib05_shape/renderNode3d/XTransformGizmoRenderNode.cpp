#include "XTransformGizmoRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <xsignal/XSignal.h>
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

	wptr<XRenderNode> mBindRenderNode;
	xsig::xconnector mConnector;
	~Internal() {
		mConnector.disconnect();
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
	float radius =0.2;
	float lineLen = 1;
	XQ::Vec2f arrowSize(0.13, 0.25);
	XQ::Vec2f lineSize(radius, lineLen);

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
	mData->mSphere->scale(lineSize[0]*1.5, lineSize[0] * 1.5, lineSize[0] * 1.5);

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

	addChild(mData->mArrowX);
	addChild(mData->mArrowY);
	addChild(mData->mArrowZ);
	addChild(mData->mSphere);
	addChild(mData->mRotateXY);
	addChild(mData->mRotateYZ);
	addChild(mData->mRotateZX);
}

void XTransformGizmoRenderNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGroupRenderNode3d::draw(parentMatrix,isNormal);
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
	mData->mConnector.disconnect();

	//同步node的位姿信息给this
	Eigen::Affine3f node_transform = node->getTransform();

	auto data = XQ::Matrix::transformDecomposition_TRS(node_transform);
	
	Eigen::Vector3f origin_pos = node_transform * Eigen::Vector3f(0, 0, 0);
	auto boundBox = node->getThisBoundBox(Eigen::Matrix4f::Identity());

	auto scale_x = boundBox.xmax - boundBox.xmin;
	auto scale_y = boundBox.ymax - boundBox.ymin;
	auto scale_z = boundBox.zmax - boundBox.zmin;
	data.sx = scale_x;
	data.sy = scale_y;
	data.sz = scale_z;
	m_transform.matrix() = XQ::Matrix::computeMatrix(data);


	//绑定函数
	mData->mConnector.connect(this,&XTransformGizmoRenderNode::SigMatrixChanged,[](const Eigen::Matrix4f& m){
		Eigen::Affine3f trans;
		trans.matrix() =m;

		Eigen::Vector3f origin_pos = trans*Eigen::Vector3f(0,0,0);
	
	});
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
