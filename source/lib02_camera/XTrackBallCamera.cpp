#include "XTrackBallCamera.h"
#include "lib00_utilty/XUtilty.h"
#include <dataBase/XVector.h>
#include<iostream>

XTrackBallCamera::XTrackBallCamera()
{
    setRotateCenter(0,0,0);
    m_transform.translation() << 0, 1, 1;
}

XTrackBallCamera::~XTrackBallCamera()
{
}

Eigen::Matrix4f XTrackBallCamera::projectionMatrix() const
{
    if (m_type == cameraType::perspective) {
        return perspective();
    }else
    return ortho();
}


//boundBox 是世界坐标系下的位置
void XTrackBallCamera::resetCamera(const double boundBox[6])
{
    //根据包围盒调整相机的位置
    Eigen::Vector3f zdir = m_transform.rotation().block(0,2,3,1);
    Eigen::Vector3f point1(boundBox[0], boundBox[1], boundBox[2]);
    Eigen::Vector3f point2(boundBox[3], boundBox[4], boundBox[5]);

    Eigen::Vector3f center = (point1 + point2)*0.5;
    float radius =(point1 - point2).norm() * 0.5+m_znear;
    float tmp = radius;

    //如果是透视投影
    if (m_type == cameraType::perspective) {
        m_fovy = 45;
        auto angle = m_fovy * 0.5;
        radius = radius / tanf(XQ::Matrix::radian(angle));
    }
    
    Eigen::Vector3f position = center +zdir * radius;
    setRotateCenter(center[0],center[1],center[2]);
    //m_cameraPos = position;
    m_transform.translation() <<position;

    updateCameraFrame();

    //此处应该根据宽高比设置
    //应该保证最小的宽或者高 >radius
    if(m_aspect >1)    
        setHeight(1.1 * radius);
    else
        setHeight(1.1 * radius / m_aspect);
    //near far调整
    if (m_type == cameraType::ortho) {
        m_zfar = radius * 2 + m_znear;
    }
    else {
        //透视投影
        m_zfar = radius + tmp;
    }
}

/**
 * @brief 
 * @param sacleFactor 缩放因子，表示缩放为最初的或上一次的多少倍
 */
void XTrackBallCamera::scale(float sacleFactor)
{
    //修改视场角度
    if (m_type == cameraType::perspective) {
        //缩放通过调整相机位置来实现,放大则靠近焦点 缩小则远离焦点
        //有两个注意点
        //1.相机靠近焦点时候，不应该过近，原则上相机点与焦点的距离应该大于near
        //2.相机远离焦点的时候，整个视锥在同步移动，当相机距离焦点太远的时候，可能会导致焦点跑到远平面的后方，导致被裁剪
        auto cameraPos = getPosition();
        auto dis = cameraPos - getRotateCenter();
        
        auto tmp = cameraPos - dis * (sacleFactor - 1);
        auto newDir =tmp - getRotateCenter();
        if(newDir.dot(dis)>0 &&newDir.norm()>getZnear()){
            //如果相机在远离，应该增发zfar距离
            cameraPos = tmp;
            m_transform.translation() <<cameraPos;
            m_zfar = std::max(m_zfar, cameraPos.norm() + 10);
            //updateCameraFrame();
           
        }else{
            //相机不可以移动到焦点的后方
        }
    }
    else {
		//如果是正交投影,则需要修改m_height
        setHeight(m_height / sacleFactor);
    } 
}

void XTrackBallCamera::setEyeDir(const XQ::Vec3f& dir) {
    //调整相机的朝向 TODO
    auto data =  XQ::Matrix::transformDecomposition_TRS(m_transform);

    Eigen::Matrix4f rotate = Eigen::Matrix4f::Identity();
    Eigen::Vector3f zdir =Eigen::Vector3f(dir.x(), dir.y(), dir.z());
    auto cos = zdir.dot(Eigen::Vector3f::UnitY());

    Eigen::Vector3f xdir = Eigen::Vector3f::UnitX();
    Eigen::Vector3f ydir = Eigen::Vector3f::UnitY();
 
    if (abs( cos) >= 0.999) {
        if (cos > 0) {
            zdir = Eigen::Vector3f::UnitY();
            ydir = -Eigen::Vector3f::UnitZ();
            xdir = Eigen::Vector3f::UnitX();
        }
        else {
            zdir = -Eigen::Vector3f::UnitY();
            ydir = Eigen::Vector3f::UnitZ();
            xdir = Eigen::Vector3f::UnitX();
        }
    }
    else {
		xdir = Eigen::Vector3f::UnitY().cross(zdir);
		ydir = zdir.cross(xdir);
    }

    xdir.normalize();
    ydir.normalize();
    zdir.normalize();

    Eigen::Vector3f new_zdir = zdir;
    Eigen::Vector3f new_xdir = new_zdir.cross(ydir).normalized();
    Eigen::Vector3f new_ydir = new_zdir.cross(new_xdir).normalized();

    rotate.block(0, 0, 3, 1) = new_xdir;
    rotate.block(0, 1, 3, 1) = new_ydir;
    rotate.block(0, 2, 3, 1) = new_zdir;

    Eigen::Matrix4f m=  XQ::Matrix::transltae(data.tx,data.ty,data.tz).matrix() * rotate * XQ::Matrix::scale(data.sx,   data.sy,   data.sz).matrix();
    m_transform.matrix() = m;
}

void XTrackBallCamera::transformUpright(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP)
{
    //fps风格相机 应该保证相机不侧翻,所以应该是绕着世界坐标系的Y轴旋转,然后绕着自身的X坐标系旋转
    Eigen::Vector2f dis = lastPoint-curPoint;
    auto dx = dis[0];
    auto dy = dis[1];

	if (isRotate) {
		abs(dx) > abs(dy) ? dy = 0 : dx = 0;

		float normalizedX = dx / width;
		float normalizedY = dy / height;

		auto m_sensitivity = 1; //灵敏度
		float rotationSpeed = 0.5f * m_sensitivity;

		double yangle = normalizedX * rotationSpeed;
		double xangle = -normalizedY * rotationSpeed;

		//相机绕着旋转中心旋转，旋转轴是世界坐标系的Y轴
        Eigen::Affine3f rotateCenterAffine = Eigen::Affine3f::Identity();
        rotateCenterAffine.translate(getRotateCenter());

		auto ry = XQ::Matrix::rotate(yangle * 180.0 / 3.14, Eigen::Vector3f::UnitY());

        Eigen::Vector3f camera_xdir= m_transform.matrix().block(0, 0, 3, 1);
		auto rx = XQ::Matrix::rotate(xangle * 180.0 / 3.14, camera_xdir);


        //整体移动，绕着
        auto tmp = m_transform;
        tmp = rotateCenterAffine * ry * rotateCenterAffine.inverse() * tmp;
        m_transform = rotateCenterAffine * rx * rotateCenterAffine.inverse() * tmp;


        //如果有必要需要调整远处的平面
        m_zfar =std::max(m_zfar, getPosition().norm() + 10);
	}
	else {
		//平移 相机自身坐标系平移
        auto cameraPos = getPosition();
        auto rotate_center = getRotateCenter();
        auto foucs = ComputeWorldToDisplay(getRotateCenter());
        Eigen::Vector3f normlize_p1= Eigen::Vector3f(curPoint[0] / width, curPoint[1] / height, foucs.z());
        Eigen::Vector3f normlize_p2 = Eigen::Vector3f(lastPoint[0] / width, lastPoint[1] / height, foucs.z());


		Eigen::Vector3f _last = ComputeDisplayToWorld(normlize_p2);
		Eigen::Vector3f _start = ComputeDisplayToWorld(normlize_p1);
		Eigen::Vector3f space =  _last-_start;

        cameraPos +=space;
        rotate_center +=space;
        setRotateCenter(rotate_center[0],rotate_center[1],rotate_center[2]);
        m_transform.translation() << cameraPos;
	}
}

void XTrackBallCamera::transformFreely(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP)
{
	//变换到[-1,1]
	Eigen::Vector3f point1;
	Eigen::Vector3f point2;
	if (isYUP) {
		point1 = Eigen::Vector3f(curPoint[0] / width, curPoint[1] / height, 0);
		point2 = Eigen::Vector3f(lastPoint[0] / width, lastPoint[1] / height, 0);
	}
	else {
		point1 = Eigen::Vector3f(curPoint[0] / width, 1 - curPoint[1] / height, 0);
		point2 = Eigen::Vector3f(lastPoint[0] / width, 1 - lastPoint[1] / height, 0);
	}

	Eigen::Vector3f rotateDir;
	float rotateAngle = 0;

	if (isRotate) {
		point1[0] = 2 * point1[0] - 1;
		point1[1] = 2 * point1[1] - 1;

		point2[0] = 2 * point2[0] - 1;
		point2[1] = 2 * point2[1] - 1;

		point1[2] = XQ::Matrix::tb_project_to_sphere(point1[0], point1[1]);
		point2[2] = XQ::Matrix::tb_project_to_sphere(point2[0], point2[1]);

		rotateDir = point1.cross(point2);
		auto len = rotateDir.norm();
		rotateAngle = asinf(len);
		rotateDir.normalize();
	}

	//平移计算
	auto foucs = ComputeWorldToDisplay(getRotateCenter());
	//foucs[2] =2* foucs[2]-1;

	Eigen::Vector3f last(point2.x(), point2.y(), foucs.z());
	Eigen::Vector3f start(point1.x(), point1.y(), foucs.z());
	Eigen::Vector3f _last = ComputeDisplayToCamera(start);
	Eigen::Vector3f _start = ComputeDisplayToCamera(last);
	Eigen::Vector3f space = _start - _last;

	auto yangle = curPoint.x() - lastPoint.x();
	auto xangle = curPoint.y() - lastPoint.y();

	if (isRotate == true) {
		space[0] = 0;
		space[1] = 0;
		space[2] = 0;

		if (abs(xangle) > abs(yangle)) {
			yangle = 0;
		}
		else
		{
			xangle = 0;
		}
		yangle *= 0.02;
		xangle *= 0.02;
	}
	else {
		rotateAngle = 0;
		xangle = 0;
		yangle = 0;
	}

	//相机绕着自身坐标系运动,旋转顺序Z->X->Y
	Eigen::Vector3f self2Center = m_transform.inverse() * getRotateCenter();
	m_transform
		.translate(self2Center)
		.rotate(Eigen::AngleAxisf(xangle, Eigen::Vector3f::UnitX()))
		.rotate(Eigen::AngleAxisf(-yangle, Eigen::Vector3f::UnitY()))
		//.rotate(Eigen::AngleAxisf(rotateAngle, rotateDir))
		.scale(Eigen::Vector3f(1, 1, 1))
		.translate(-self2Center)
		.translate(space);
}


Eigen::Affine3f XTrackBallCamera::getCameraFrame() const
{
    return m_transform;
}

Eigen::Affine3f& XTrackBallCamera::getCameraFrame()
{
    return m_transform;
}

void XTrackBallCamera::updateCameraFrame()
{
    //m_cameraPos 表示相机的位置
    //m_cameraPos - rotate_center 表示相机的Z方向
    Eigen::Vector3f zdir = getPosition()-getRotateCenter();
    zdir.normalize();

	Eigen::Vector3f ydir = Eigen::Vector3f::UnitY();
	Eigen::Vector3f xdir = Eigen::Vector3f::UnitX();

	if (abs(zdir.dot(Eigen::Vector3f::UnitY())) > 0.99) {
		if (zdir.dot(Eigen::Vector3f::UnitY()) > 0) {
			ydir = -Eigen::Vector3f::UnitZ();
			xdir = Eigen::Vector3f::UnitX();
		}
		else {
			ydir = Eigen::Vector3f::UnitZ();
			xdir = Eigen::Vector3f::UnitX();
		}
	}
	else {
		xdir = Eigen::Vector3f::UnitY().cross(zdir);
		ydir = zdir.cross(xdir);
	}

    Eigen::Matrix4f frame = Eigen::Matrix4f::Identity();
    xdir.normalize();
    ydir.normalize();
    frame.block(0,0,3,1) = xdir;
    frame.block(0,1,3,1) = ydir;
    frame.block(0,2,3,1) = zdir;
    frame.block(0,3,3,1) = getPosition();

    m_transform.matrix() = frame;
}

Eigen::Vector3f XTrackBallCamera::getCameraPos() const
{
    return m_transform.translation();
}
