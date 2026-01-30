#include "XBaseCamera.h"
#include "lib00_utilty/XUtilty.h"
#include <dataBase/XVector.h>
#include<iostream>

XBaseCmaera::XBaseCmaera()
{
    rotate_center = { 0,0,0 };
}

XBaseCmaera::~XBaseCmaera()
{
}

Eigen::Matrix4f XBaseCmaera::projectionMatrix() const
{
    if (m_type == cameraType::perspective) {
        return perspective();
    }else
    return ortho();
}

Eigen::Matrix4f XBaseCmaera::perspective() const
{
    return XQ::Matrix::perspective(m_fovy, m_aspect, m_znear, m_zfar);
}

Eigen::Matrix4f XBaseCmaera::ortho() const
{
    auto height = getHeight();
    float width = m_aspect * height;
    return XQ::Matrix::ortho(-0.5*width,0.5*width,-0.5* height,0.5* height, m_znear, m_zfar);
}


std::vector<XQ::Vec3f> XBaseCmaera::getFrustumInWorld() const
{
    //获取视锥体的八个顶点
    std::vector<XQ::Vec3f> frustum;
    Eigen::Matrix4f view = getViewMatrix();
    Eigen::Matrix4f proj = projectionMatrix();
    Eigen::Matrix4f viewProj = proj * view;
    auto ss =  view.inverse() * proj.inverse();
    //先近平面，后远平面 逆时针旋转
    Eigen::Vector4f points[8] = {
        ss* Eigen::Vector4f(-1, -1, -1, 1), 
        ss* Eigen::Vector4f(1, -1, -1, 1),
        ss* Eigen::Vector4f(1, 1, -1, 1),
        ss* Eigen::Vector4f(-1, 1, -1, 1),
        ss* Eigen::Vector4f(-1, -1, 1, 1),
        ss* Eigen::Vector4f(1, -1, 1, 1),
        ss* Eigen::Vector4f(1, 1, 1, 1),
        ss* Eigen::Vector4f(-1, 1, 1, 1)
    };

    auto ss2= proj.inverse();
	Eigen::Vector4f points2[8] = {
		ss2 * Eigen::Vector4f(-1, -1, -1, 1),
		ss2 * Eigen::Vector4f(1, -1, -1, 1),
		ss2 * Eigen::Vector4f(1, 1, -1, 1),
		ss2* Eigen::Vector4f(-1, 1, -1, 1),
		ss2* Eigen::Vector4f(-1, -1, 1, 1),
		ss2* Eigen::Vector4f(1, -1, 1, 1),
		ss2* Eigen::Vector4f(1, 1, 1, 1),
		ss2* Eigen::Vector4f(-1, 1, 1, 1)
	};

    std::vector<XQ::Vec3f> frustum2;
	for (int i = 0; i < 8; i++) {
		XQ::Vec3f point(points2[i].x() / points2[i].w(), points2[i].y() / points2[i].w(), points2[i].z() / points2[i].w());
        frustum2.push_back(point);
	}

    for (int i = 0; i < 8; i++) {
        XQ::Vec3f point(points[i].x() / points[i].w(), points[i].y() / points[i].w(), points[i].z() / points[i].w());
        frustum.push_back(point);
    }

    return frustum;
}

XQ::Vec3f XBaseCmaera::getNearPointInWorld() const
{
    Eigen::Vector3f points(0,0,-m_znear);
    Eigen::Vector3f out = getCameraFrame()*points;
    return XQ::Vec3f(out.x(), out.y(), out.z());
}

XQ::Vec3f XBaseCmaera::getFarPointInWorld() const
{
	Eigen::Vector3f points(0, 0, -m_zfar);
	Eigen::Vector3f out = getCameraFrame() * points;
	return XQ::Vec3f(out.x(), out.y(), out.z());
}


Eigen::Matrix4f XBaseCmaera::getViewMatrix() const
{
    return getCameraFrame().inverse().matrix();
}

Eigen::Vector3f XBaseCmaera::billboard(float screenw, float screenh, float posx, float posy, float zInCamera, oriention orien )
{
    float ratiow=0; 
    float ratioh=0;
    if (orien == oriention::leftBottom) {
        ratiow = posx / screenw;
        ratioh = posy / screenh;
    }

    if (orien == oriention::leftTop) {
        ratiow = posx / screenw;
        ratioh = 1-posy / screenh;
    }

    if (orien == oriention::rightBottom) {
        ratiow =1- posx / screenw;
        ratioh = posy / screenh;
    }

    if (orien == oriention::rightTop) {
        ratiow = 1 - posx / screenw;
        ratioh = 1-posy / screenh;
    }
    
    float ratioz = zInCamera / (m_zfar - m_znear);
    Eigen::Vector4f ret = projectionMatrix()* Eigen::Vector4f(0, 0, -zInCamera, 1);

    float ndc_z = ret.z() / ret.w();
    float ndc_x = ratiow * 2 - 1;
    float ndc_y = ratioh * 2 - 1;
  
    Eigen::Vector4f out = projectionMatrix().inverse() * Eigen::Vector4f(ndc_x, ndc_y, ndc_z, 1);

    return Eigen::Vector3f(out.x() / out.w(), out.y() / out.w(), out.z() / out.w());
}

float XBaseCmaera::scaleFactorH(float zValue, float screenw)
{
    if (m_type == cameraType::ortho) {
        float width = m_aspect * getHeight();
        return width / screenw;
    }
    else {
        float half_height = zValue * tanf(XQ::Matrix::radian(0.5 * m_fovy));
        float height = 2 * half_height;
        float width = m_aspect * height;
        return width / screenw;
    }
}

float XBaseCmaera::scaleFactorHZnear(float screenw)
{
    return scaleFactorH(m_znear, screenw);
}

float XBaseCmaera::scaleFactorV(float zValue, float screenh)
{
    if (m_type == cameraType::ortho) {
        return getHeight() / screenh;
    }
    else {
        float half_height = zValue * tanf(XQ::Matrix::radian(0.5 * m_fovy));
        float height = 2 * half_height;
        return height / screenh;
    }
}

float XBaseCmaera::scaleFactorVZnear(float screenh)
{
    return scaleFactorV(m_znear, screenh);
}

void XBaseCmaera::setRotateCenter(float x, float y, float z)
{
    //重新设置旋转中心
    rotate_center = Eigen::Vector3f(x, y, z);
}

Eigen::Vector3f XBaseCmaera::getRotateCenter() const
{
    return rotate_center;
}


Eigen::Vector3f XBaseCmaera::ComputeDisplayToNdc(Eigen::Vector3f input) const
{
	Eigen::Affine3f screenMap = Eigen::Affine3f::Identity();
	screenMap.translate(Eigen::Vector3f(1,1,1)) ;
	screenMap.scale(Eigen::Vector3f(0.5, 0.5, 0.5 * 1));
    Eigen::Affine3f  fff = screenMap.inverse();
    Eigen::Vector3f ndc1 = fff * input;
    
    //x*2-1

	//Eigen::Vector3f ndc = screenMap.inverse() * input;
	Eigen::Vector3f ndc = Eigen::Vector3f( input.x()*2.f-1, input.y()*2.f-1, input.z() * 2.f - 1);

	return ndc;
}

Eigen::Vector3f XBaseCmaera::ComputeNdcToDisplay(Eigen::Vector3f input) const
{
	Eigen::Affine3f screenMap = Eigen::Affine3f::Identity();
	screenMap.translation() << 1, 1, 1;
	screenMap.scale(Eigen::Vector3f(0.5, 0.5, 0.5 * 1));

	//Eigen::Vector3f display = screenMap* input;

    Eigen::Vector3f display = Eigen::Vector3f((input.x() +1)*0.5, (input.y() + 1) * 0.5, (input.z() + 1) * 0.5);

	return display;
}

Eigen::Vector3f XBaseCmaera::ComputeWorldToDisplay(Eigen::Vector3f input) const
{
	Eigen::Vector4f ndc = projectionMatrix() * getViewMatrix() * Eigen::Vector4f(input.x(), input.y(), input.z(), 1);

	ndc /= ndc.w();

    return ComputeNdcToDisplay(Eigen::Vector3f(ndc.x(), ndc.y(), ndc.z()));
}

Eigen::Vector3f XBaseCmaera::ComputeDisplayToWorld(Eigen::Vector3f input) const
{
	Eigen::Vector3f ndc = ComputeDisplayToNdc(input);

	Eigen::Vector4f  world = getViewMatrix().inverse() * projectionMatrix().inverse() * Eigen::Vector4f(ndc.x(), ndc.y(), ndc.z(), 1);
	world /= world.w();
	return Eigen::Vector3f(world.x(), world.y(), world.z());
}

Eigen::Vector3f XBaseCmaera::ComputeDisplayToCamera(Eigen::Vector3f input) const
{
	Eigen::Vector3f ndc = ComputeDisplayToNdc(input);

	Eigen::Vector4f  world = projectionMatrix().inverse() * Eigen::Vector4f(ndc.x(), ndc.y(), ndc.z(), 1);
	world /= world.w();
	return Eigen::Vector3f(world.x(), world.y(), world.z());
}

Eigen::Vector3f XBaseCmaera::ComputeWorldToCamera(Eigen::Vector3f input) const
{
    Eigen::Vector4f ret = getViewMatrix()* Eigen::Vector4f(input.x(), input.y(), input.z(), 1);
    ret /= ret.w();
    return Eigen::Vector3f(ret.x(), ret.y(), ret.z());
}

Eigen::Vector3f XBaseCmaera::ComputeCameraToWorld(Eigen::Vector3f input) const
{
    Eigen::Vector4f ret = getViewMatrix().inverse() * Eigen::Vector4f(input.x(), input.y(), input.z(), 1);
    ret /= ret.w();
    return Eigen::Vector3f(ret.x(), ret.y(), ret.z());
}

Eigen::Vector3f XBaseCmaera::getPosition() const
{
    return getCameraFrame().translation();
}