#include "XTransformGizmoRenderNode.h"
#include <lib04_opengl/XOpenGLEnable.h>
#include <lib01_shader/xshaderManger.h>
#include <lib04_opengl/XOpenGLBuffer.h>

XTransformGizmoRenderNode::XTransformGizmoRenderNode()
{

}

XTransformGizmoRenderNode::~XTransformGizmoRenderNode()
{
}



void XTransformGizmoRenderNode::Init()
{
	XGroupRenderNode3d::Init();
	float radius =2;
	float lineLen = 20;
	XQ::Vec2f arrowSize(3, 5);
	XQ::Vec2f lineSize(radius, lineLen);

	mArrowZ = makeShareDbObject<XArrowRenderNode>();
	mArrowZ->setSingleColor(XQ::Vec4f(0, 0, 1, 1));
	mArrowZ->setPolygonMode(PolygonMode::all);
	mArrowZ->setColorMode(ColorMode::SingleColor);
	mArrowZ->setLineSize(lineSize[0],lineSize[1]);
	mArrowZ->setArrowSize(arrowSize[0], arrowSize[1]);

	mArrowX = makeShareDbObject<XArrowRenderNode>();
	mArrowX->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
	mArrowX->setPolygonMode(PolygonMode::all);
	mArrowX->setColorMode(ColorMode::SingleColor);
	mArrowX->setLineSize(lineSize[0], lineSize[1]);
	mArrowX->setArrowSize(arrowSize[0], arrowSize[1]);
	mArrowX->rotateY(90);

	mArrowY = makeShareDbObject<XArrowRenderNode>();
	mArrowY->setSingleColor(XQ::Vec4f(0, 1, 0, 1));
	mArrowY->setPolygonMode(PolygonMode::all);
	mArrowY->setColorMode(ColorMode::SingleColor);
	mArrowY->setLineSize(lineSize[0], lineSize[1]);
	mArrowY->setArrowSize(arrowSize[0], arrowSize[1]);
	mArrowY->rotateX(270);

	mSphere = makeShareDbObject<XSphereRenderNode>();
	mSphere->scale(lineSize[0]*1.5, lineSize[0] * 1.5, lineSize[0] * 1.5);

	mRotateXY = makeShareDbObject<XTorusRenderNode>();
	mRotateXY->setEndAngle(90);
	mRotateXY->setMajorRadius(lineLen*0.5);
	mRotateXY->setMinorRadius(radius);

	mRotateYZ = makeShareDbObject<XTorusRenderNode>();
	mRotateYZ->setEndAngle(90);
	mRotateYZ->setMajorRadius(lineLen * 0.5);
	mRotateYZ->setMinorRadius(radius);
	mRotateYZ->rotateY(-90);

	mRotateZX = makeShareDbObject<XTorusRenderNode>();
	mRotateZX->setEndAngle(90);
	mRotateZX->setMajorRadius(lineLen * 0.5);
	mRotateZX->setMinorRadius(radius);
	mRotateZX->rotateX(90);

	addChild(mArrowX);
	addChild(mArrowY);
	addChild(mArrowZ);
	addChild(mSphere);
	addChild(mRotateXY);
	addChild(mRotateYZ);
	addChild(mRotateZX);
}

void XTransformGizmoRenderNode::draw(const Eigen::Matrix4f& parentMatrix, bool isNormal)
{
	XGroupRenderNode3d::draw(parentMatrix,isNormal);
}