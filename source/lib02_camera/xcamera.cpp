#include "xcamera.h"
#include "lib00_utilty/XUtilty.h"
#include <dataBase/XVector.h>
#include<iostream>

xcamera::xcamera()
{
    rotate_center = { 0,0,0 };
}

xcamera::~xcamera()
{
}

Eigen::Matrix4f xcamera::projectionMatrix() const
{
    if (m_type == cameraType::perspective) {
        return perspective();
    }else
    return ortho();
}

Eigen::Matrix4f xcamera::perspective() const
{
    return XQ::Matrix::perspective(m_fovy, m_aspect, m_znear, m_zfar);
}

Eigen::Matrix4f xcamera::ortho() const
{
    auto height = getHeight();
    float width = m_aspect * height;
    return XQ::Matrix::ortho(-0.5*width,0.5*width,-0.5* height,0.5* height, m_znear, m_zfar);
}

//boundBox 是世界坐标系下的位置
void xcamera::resetCamera(const double boundBox[6])
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
    rotate_center = center;
    m_cameraPos = position;

    #if 0
	Eigen::Vector3f xdir;
	Eigen::Vector3f ydir;

    //重新构建坐标系
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

   
    xdir.normalize();
    ydir.normalize();
    zdir.normalize();

    
    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    m.block(0, 0, 3, 1) = xdir;
    m.block(0, 1, 3, 1) = ydir;
    m.block(0, 2, 3, 1) = zdir;
    m.block(0, 3, 3, 1) = position;
    m_transform.matrix() = m;
    
    #else
    updateCameraFrame();
    #endif

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

std::vector<XQ::Vec3f> xcamera::getFrustumInWorld() const
{
    //获取视锥体的八个顶点
    std::vector<XQ::Vec3f> frustum;
    Eigen::Matrix4f view = m_transform.matrix().inverse();
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

XQ::Vec3f xcamera::getNearPointInWorld() const
{
    Eigen::Vector3f points(0,0,-m_znear);
    Eigen::Vector3f out =m_transform*points;
    return XQ::Vec3f(out.x(), out.y(), out.z());
}

XQ::Vec3f xcamera::getFarPointInWorld() const
{
	Eigen::Vector3f points(0, 0, -m_zfar);
	Eigen::Vector3f out = m_transform * points;
	return XQ::Vec3f(out.x(), out.y(), out.z());
}

/**
 * @brief 
 * @param sacleFactor 缩放因子，表示缩放为最初的或上一次的多少倍
 */
void xcamera::scale(float sacleFactor, bool isLast)
{
    //修改视场角度
    if (m_type == cameraType::perspective) {
        #if 0
		float angle = 0;
		if (isLast)
			angle = m_fovy * 0.5;
		else
			angle = 45 * 0.5;
		auto new_half_fovy = XQ::Matrix::Angle(atanf(tanf(XQ::Matrix::radian(angle)) / sacleFactor));
		if (2 * new_half_fovy >= 170 || 2 * new_half_fovy < 1)
			return;
		m_fovy = 2 * new_half_fovy;
        #endif
        //缩放通过调整相机位置来实现,放大则靠近焦点 缩小则远离焦点
        //有两个注意点
        //1.相机靠近焦点时候，不应该过近，原则上相机点与焦点的距离应该大于near
        //2.相机远离焦点的时候，整个视锥在同步移动，当相机距离焦点太远的时候，可能会导致焦点跑到远平面的后方，导致被裁剪
        auto dis =m_cameraPos - rotate_center;
        
        auto tmp = m_cameraPos- dis * (sacleFactor - 1);
        auto newDir =tmp -rotate_center;
        if(newDir.dot(dis)>0 &&newDir.norm()>getZnear()){
            //如果相机在远离，应该增发zfar距离
            m_cameraPos = tmp;
            
            m_zfar = std::max(m_zfar, m_cameraPos.norm() + 10);

            updateCameraFrame();
           
        }else{
            //相机不可以移动到焦点的后方
        }
    }
    else {
		//如果是正交投影,则需要修改m_height
        setHeight(m_height / sacleFactor);
    } 
}

void xcamera::transform(float picth, float yaw, float roll, float tx, float ty, float tz)
{
    transformImpl(picth, yaw, roll,tx,ty,tz);
}

void xcamera::setEyeDir(const XQ::Vec3f& dir) {
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

void xcamera::transformFreely(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint,float width,float height,  bool isRotate,bool isYUP)
{
    //变换到[-1,1]
	Eigen::Vector3f point1;
	Eigen::Vector3f point2;
    if (isYUP) {
		 point1 = Eigen::Vector3f(curPoint[0] / width,  curPoint[1] / height, 0);
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
    #if 1
    //相机绕着自身坐标系运动,旋转顺序Z->X->Y
    Eigen::Vector3f self2Center = m_transform.inverse() * rotate_center;
    m_transform
        .translate(self2Center)
		.rotate(Eigen::AngleAxisf(xangle, Eigen::Vector3f::UnitX()))
		.rotate(Eigen::AngleAxisf(-yangle, Eigen::Vector3f::UnitY()))
        //.rotate(Eigen::AngleAxisf(rotateAngle, rotateDir))
        .scale(Eigen::Vector3f(1, 1, 1))
        .translate(-self2Center)
        .translate(space);
    #else
        //先计算姿态，后计算位置
        //相机先绕着世界坐标系的Y轴旋转，然后绕着自身的X坐标系旋转，这样可以保证不侧翻
        
        //提取姿态
    {
        Eigen::Affine3f t = m_transform;
		Eigen::Vector3f self2Center = t.inverse() * rotate_center;
        t
			.translate(self2Center)
			.rotate(Eigen::AngleAxisf(XQ::Matrix::radian( xangle * 180.0 / 3.14), Eigen::Vector3f::UnitX()))
			.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(-yangle * 180.0 / 3.14), Eigen::Vector3f::UnitY()))
			//.rotate(Eigen::AngleAxisf(rotateAngle, rotateDir))
			.scale(Eigen::Vector3f(1, 1, 1))
			.translate(-self2Center);
		std::cout << "t\n";
		XQ::Matrix::dump(t, std::cout);
    }
    

        auto cameraRot =XQ::Matrix::rotate(m_transform);
		{
            auto cameraRotTest =XQ::Matrix::rotate(m_transform);
			Eigen::Vector3f diry = cameraRotTest * Eigen::Vector3f::UnitY();
			Eigen::Vector3f dirx = cameraRotTest * Eigen::Vector3f::UnitX();

			auto rx = XQ::Matrix::rotate(-xangle * 180.0 / 3.14, dirx);
			auto ry = XQ::Matrix::rotate(yangle * 180.0 / 3.14, diry);

			Eigen::Affine3f world = ry * rx;
            cameraRotTest = world.inverse() * cameraRotTest;

            std::cout<<"cameraRotTest\n";
            XQ::Matrix::dump(cameraRotTest, std::cout);
		}
        
        {
            auto rx = XQ::Matrix::rotate(-xangle * 180.0 / 3.14, Eigen::Vector3f::UnitX());
            auto ry = XQ::Matrix::rotate(yangle * 180.0 / 3.14, Eigen::Vector3f::UnitY());
            Eigen::Affine3f  a= cameraRot* ry* rx* cameraRot.inverse();
            cameraRot = a.inverse() * cameraRot;

            //cameraRot *rx逆*ry逆*cameraRot的逆*cameraRot = cameraRot *rx逆*ry逆
            std::cout << "cameraRot\n";
            XQ::Matrix::dump(cameraRot, std::cout);
        }


        //求解平移部分
        Eigen::Vector3f self2Center = m_transform.inverse() * rotate_center;
        Eigen::Vector3f T = rotate_center - cameraRot*self2Center;

        //合并R T
        cameraRot.translation()<<T.x(),T.y(),T.z();
        m_transform.matrix() = cameraRot.matrix();

        {
			std::cout << "m_transform\n";
			XQ::Matrix::dump(m_transform, std::cout);
        }

    #endif
}

void xcamera::transformUpright(Eigen::Vector2f curPoint, Eigen::Vector2f lastPoint, float width, float height, bool isRotate, bool isYUP)
{
    //fps风格相机 应该保证相机不侧翻,所以应该是绕着世界坐标系的Y轴旋转,然后绕着自身的X坐标系旋转
    Eigen::Vector2f dis = lastPoint-curPoint;
    auto dx = dis[0];
    auto dy = dis[1];

    #if 0
    if (isRotate) {
		abs(dx) > abs(dy) ? dy = 0 : dx = 0;

		float normalizedX = dx / width;
		float normalizedY = dy / height;

        auto m_sensitivity = 1; //灵敏度
        float rotationSpeed = 0.5f * m_sensitivity;

		double yangle = normalizedX * rotationSpeed;
		double xangle = -normalizedY * rotationSpeed;

		auto cameraRot = XQ::Matrix::rotate(m_transform);

		auto ry = XQ::Matrix::rotate(yangle * 180.0 / 3.14, Eigen::Vector3f::UnitY());
		auto rx = XQ::Matrix::rotate(xangle * 180.0 / 3.14, Eigen::Vector3f::UnitX());
		auto rot = ry * cameraRot * rx;

		Eigen::Vector3f T = m_transform.translation();
		//合并R T
		rot.translation() << T.x(), T.y(), T.z();
		m_transform.matrix() = rot.matrix();
        //std::cout<<"xxx:"<< cameraRot.matrix();
    }
    else {
        //平移 相机自身坐标系平移
    }
    #endif
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
        rotateCenterAffine.translate(rotate_center);

		auto ry = XQ::Matrix::rotate(yangle * 180.0 / 3.14, Eigen::Vector3f::UnitY());

        Eigen::Vector3f camera_xdir= m_transform.matrix().block(0, 0, 3, 1);
		auto rx = XQ::Matrix::rotate(xangle * 180.0 / 3.14, camera_xdir);

		//m_cameraPos = rotateCenterAffine * ry * rotateCenterAffine.inverse() * m_cameraPos;
		//m_cameraPos = rotateCenterAffine * rx * rotateCenterAffine.inverse() * m_cameraPos;


        //整体移动，绕着
        auto tmp = m_transform;
        tmp = rotateCenterAffine * ry * rotateCenterAffine.inverse() * tmp;
        m_transform = rotateCenterAffine * rx * rotateCenterAffine.inverse() * tmp;

        //Eigen::Vector3f cameraPos = m_transform.translation();
        //m_transform.translation()<< m_cameraPos;
        //updateCameraFrame();

        //如果有必要需要调整远处的平面
        m_zfar =std::max(m_zfar, m_cameraPos.norm() + 10);
	}
	else {
		//平移 相机自身坐标系平移
        auto foucs = ComputeWorldToDisplay(getRotateCenter());
        Eigen::Vector3f normlize_p1= Eigen::Vector3f(curPoint[0] / width, curPoint[1] / height, foucs.z());
        Eigen::Vector3f normlize_p2 = Eigen::Vector3f(lastPoint[0] / width, lastPoint[1] / height, foucs.z());


		Eigen::Vector3f _last = ComputeDisplayToWorld(normlize_p2);
		Eigen::Vector3f _start = ComputeDisplayToWorld(normlize_p1);
		Eigen::Vector3f space =  _last-_start;

        m_cameraPos +=space;
        rotate_center +=space;
        m_transform.translation() << m_cameraPos;
        //updateCameraFrame();
	}
}

Eigen::Vector3f xcamera::billboard(float screenw, float screenh, float posx, float posy, float zInCamera, oriention orien )
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

float xcamera::scaleFactorH(float zValue, float screenw)
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

float xcamera::scaleFactorHZnear(float screenw)
{
    return scaleFactorH(m_znear, screenw);
}

float xcamera::scaleFactorV(float zValue, float screenh)
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

float xcamera::scaleFactorVZnear(float screenh)
{
    return scaleFactorV(m_znear, screenh);
}

void xcamera::transformImpl(float pitch, float yaw, float roll, float tx, float ty, float tz)
{
    Eigen::Vector3f self2Center =m_transform.inverse()* rotate_center;

    //相机绕着自身坐标系运动,旋转顺序Z->X->Y
    m_transform
        .translate(self2Center)
        .rotate(Eigen::AngleAxisf(XQ::Matrix::radian(roll), Eigen::Vector3f::UnitZ()))
        .rotate(Eigen::AngleAxisf(XQ::Matrix::radian(pitch), Eigen::Vector3f::UnitX()))
        .rotate(Eigen::AngleAxisf(XQ::Matrix::radian(yaw), Eigen::Vector3f::UnitY()))
        .scale(Eigen::Vector3f(1,1,1))
        .translate(-self2Center)
        .translate(Eigen::Vector3f(tx, ty, tz));

    //当前姿态A 绕B定轴旋转 Z->X->Y 
    //B*RY*RX*RZ*B逆A => A*A逆 * B *RY*RX*RZ*(A逆 * B)的逆
}

void xcamera::setTranslate(float x, float y, float z)
{
    m_transform.translation() << x, y, z;
}

void xcamera::setRotateCenter(float x, float y, float z)
{
    //重新设置旋转中心
    rotate_center = Eigen::Vector3f(x, y, z);
}

Eigen::Vector3f xcamera::getRotateCenter() const
{
    return rotate_center;
}

Eigen::Matrix4f xcamera::getViewMatrix() const
{
    return m_transform.inverse().matrix();
}

Eigen::Vector3f xcamera::ComputeDisplayToNdc(Eigen::Vector3f input) const
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

Eigen::Vector3f xcamera::ComputeNdcToDisplay(Eigen::Vector3f input) const
{
	Eigen::Affine3f screenMap = Eigen::Affine3f::Identity();
	screenMap.translation() << 1, 1, 1;
	screenMap.scale(Eigen::Vector3f(0.5, 0.5, 0.5 * 1));

	//Eigen::Vector3f display = screenMap* input;

    Eigen::Vector3f display = Eigen::Vector3f((input.x() +1)*0.5, (input.y() + 1) * 0.5, (input.z() + 1) * 0.5);

	return display;
}

Eigen::Vector3f xcamera::ComputeWorldToDisplay(Eigen::Vector3f input) const
{
	Eigen::Vector4f ndc = projectionMatrix() * getViewMatrix() * Eigen::Vector4f(input.x(), input.y(), input.z(), 1);

	ndc /= ndc.w();

    return ComputeNdcToDisplay(Eigen::Vector3f(ndc.x(), ndc.y(), ndc.z()));
}

Eigen::Vector3f xcamera::ComputeDisplayToWorld(Eigen::Vector3f input) const
{
	Eigen::Vector3f ndc = ComputeDisplayToNdc(input);

	Eigen::Vector4f  world = getViewMatrix().inverse() * projectionMatrix().inverse() * Eigen::Vector4f(ndc.x(), ndc.y(), ndc.z(), 1);
	world /= world.w();
	return Eigen::Vector3f(world.x(), world.y(), world.z());
}

Eigen::Vector3f xcamera::ComputeDisplayToCamera(Eigen::Vector3f input) const
{
	Eigen::Vector3f ndc = ComputeDisplayToNdc(input);

	Eigen::Vector4f  world = projectionMatrix().inverse() * Eigen::Vector4f(ndc.x(), ndc.y(), ndc.z(), 1);
	world /= world.w();
	return Eigen::Vector3f(world.x(), world.y(), world.z());
}

Eigen::Vector3f xcamera::ComputeWorldToCamera(Eigen::Vector3f input) const
{
    Eigen::Vector4f ret = getViewMatrix()* Eigen::Vector4f(input.x(), input.y(), input.z(), 1);
    ret /= ret.w();
    return Eigen::Vector3f(ret.x(), ret.y(), ret.z());
}

Eigen::Vector3f xcamera::ComputeCameraToWorld(Eigen::Vector3f input) const
{
    Eigen::Vector4f ret = getViewMatrix().inverse() * Eigen::Vector4f(input.x(), input.y(), input.z(), 1);
    ret /= ret.w();
    return Eigen::Vector3f(ret.x(), ret.y(), ret.z());
}

Eigen::Vector3f xcamera::getPosition() const
{
    return m_transform.translation();
}

void xcamera::updateCameraFrame()
{
    //m_cameraPos 表示相机的位置
    //m_cameraPos - rotate_center 表示相机的Z方向
    Eigen::Vector3f zdir = m_cameraPos-rotate_center;
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
    frame.block(0,3,3,1) = m_cameraPos;

    m_transform.matrix() = frame;
}
