#include "MultiPolygonModeCube.h"
#include <glew/glew.h>
#include <windows.h>
#include <Eigen/Eigen>
#include <lib01_shader/xshader.h>
#include <lib02_camera/xcamera.h>
#include <lib00_utilty/myUtilty.h>

#include <QMouseEvent> 
#include <QWheelEvent> 
#include <iostream>
#include <QDebug>

enum class CameraAction {
	None = 1,
	Rotate = 1 << 1,
	Move = 1 << 2
};

struct MultiPolygonModeCube::Internal {

	//使用多个vbo存储顶点数据
	//顶点坐标
	
	std::vector<myUtilty::Vec3f> vertextCoord{
		//坐标的方向准则，右手，大拇指指向物体中心，四指指向的方向
		//前面的四个坐标点
		myUtilty::Vec3f(-1.0f, -1.0f,  1.0f),
		myUtilty::Vec3f(-1.0f,  1.0f,  1.0f),
		myUtilty::Vec3f( 1.0f,  1.0f,  1.0f),
		myUtilty::Vec3f(1.0f, -1.0f,  1.0f),
		

		//后面的四个坐标点
		myUtilty::Vec3f(-1.0f, -1.0f, -1.0f),
		myUtilty::Vec3f( 1.0f, -1.0f, -1.0f),
		myUtilty::Vec3f( 1.0f,  1.0f, -1.0f),
		myUtilty::Vec3f(-1.0f,  1.0f, -1.0f)
	};

	std::vector<myUtilty::Vec3u> IndexData{
		//左下角 右 上
		//左下角 右上 左
		//前
		myUtilty::Vec3u(0,3,2),
		myUtilty::Vec3u(0,2,1),
		
		//后
		myUtilty::Vec3u(5,4,7),
		myUtilty::Vec3u(5,7,6),

		//左
		myUtilty::Vec3u(4,0,1),
		myUtilty::Vec3u(4,1,7),

		//右
		myUtilty::Vec3u(3,5,6),
		myUtilty::Vec3u(3,6,2),

		//上
		myUtilty::Vec3u(1,2,6),
		myUtilty::Vec3u(1,6,7),

		//下
		myUtilty::Vec3u(4,5,3),
		myUtilty::Vec3u(4,3,0)
	};

	std::vector<myUtilty::Vec4f> SurfaceColor{
		myUtilty::Vec4f(0.0f, 0.0f,1.f,1),		//前
		myUtilty::Vec4f(0.0f, 0.0f,1.f,1),		//后
		myUtilty::Vec4f(1.0f, 0.0f,0.f,1),		//左
		myUtilty::Vec4f(1.0f, 0.0f,0.f,1),		//右
		myUtilty::Vec4f(0.0f, 1.0f,0.f,1),		//上
		myUtilty::Vec4f(0.0f, 1.0f,0.f,1),		//下
	};

	//!
	//! global variable
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint SSBO;
	xshader shader;
	xcamera camera;
	Eigen::Matrix4f mat_m = Eigen::Matrix4f::Identity();
	CameraAction cameraAction = CameraAction::None;

	GLuint mTboTexture;      // tbo纹理缓存对象

	//鼠标的位置实时更新
	Eigen::Vector2f pos1 = Eigen::Vector2f(0,0);		//第一个位置
	Eigen::Vector2f pos2 = Eigen::Vector2f(0, 0);		//第二个位置
};

MultiPolygonModeCube::MultiPolygonModeCube(QWidget* parent) :XOpenGLWidget(parent),d(std::make_unique<Internal>())
{
	setFocusPolicy(Qt::StrongFocus); // 添加这行
}

MultiPolygonModeCube::~MultiPolygonModeCube()
{
}

void MultiPolygonModeCube::render()
{
	makeCurrent();

	glEnable(GL_DEPTH_TEST);
	if(!glIsEnabled(GL_MULTISAMPLE))
		glEnable(GL_MULTISAMPLE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_FLAT);
	
	// 绘制
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, d->mTboTexture);

	d->shader.use();

	d->camera.projectionMatrix();
	d->camera.getViewMatrix();

	d->mat_m(0, 0) = 0.5;
	d->mat_m(1, 1) = 0.5;
	d->mat_m(2, 2) = 0.5;

	d->shader.setMat4("m", d->mat_m);
	d->shader.setMat4("v", d->camera.getViewMatrix());
	d->shader.setMat4("p", d->camera.projectionMatrix());
	d->shader.setBool("isWireframe", false);

	glBindVertexArray(d->VAO);
	
	//glEnable(GL_POLYGON_OFFSET_FILL);    // 启用填充模式的偏移
	//glPolygonOffset(1.0f, 1.0f);         // 偏移参数（factor, units）
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//glDisable(GL_POLYGON_OFFSET_FILL);   // 禁用偏移
	d->shader.setFloat("lineWidth", 10);
	d->shader.setVec2("screenSize",Eigen::Vector2f(mWidth,mHeight));
	d->shader.setBool("isWireframe", true);
	d->shader.setVec4("wireframeColor",Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);


	//交换缓冲
	swapBuffers();
	glUseProgram(0);
	doneCurrent();
}

void MultiPolygonModeCube::initGLResource()
{
	d->shader = xshader( {
											{xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/06cube/" + "cube.vs"},						//顶点着色器
											{xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/06cube/" + "cube.fs"},				//片元着色器
											{xshader::ShaderType::GEOMETRY, myUtilty::ShareVar::instance().currentExeDir + "/06cube/" + "cube.gs"}					//几何着色器
									   });
	//!
	//! create VAO VBO EBO
	glGenVertexArrays(1, &d->VAO);
	glGenBuffers(1, &d->VBO);
	glGenBuffers(1, &d->EBO);

	//!
	//! bind vbo ebo to vao
	glBindVertexArray(d->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, d->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);

	//!
	//! allocate data to vbo
	glBindBuffer(GL_ARRAY_BUFFER, d->VBO);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(myUtilty::Vec3f), d->vertextCoord.data(), GL_STATIC_DRAW);

	//!
	//! allocate data to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(myUtilty::Vec3u), d->IndexData.data(), GL_STATIC_DRAW);

	//!
	//! set vao attribute
	glBindVertexArray(d->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(myUtilty::Vec3u), (void*)0);
	glEnableVertexAttribArray(0);

	//!
	//! SSBO 存储颜色数据
	glGenBuffers(1, &d->SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, d->SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(myUtilty::Vec4f), d->SurfaceColor.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, d->SSBO);

	//!
	//! create texture
	glGenTextures(1, &d->mTboTexture);																//创建纹理缓存对象
	glBindTexture(GL_TEXTURE_BUFFER, d->mTboTexture);								//绑定纹理缓存对象
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, d->SSBO);						//将颜色数据存储到纹理缓存对象
	glBindTexture(GL_TEXTURE_BUFFER, 0);														//解绑纹理缓存对象

	//!
	//! unbind vao ebo 解绑VBO EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//!
	//! reset camera
	double boundBox[6] {-1,-1,-1,1,1,1};
	d->camera.resetCamera(boundBox);
}

void MultiPolygonModeCube::mousePressEvent(QMouseEvent* event)
{
	//std::cout<<std::dec<< "鼠标摁下: (" << event->pos().x() << "," << event->pos().y() << ")" << std::endl;
	//左键旋转 右键平移  中键缩放
	if (event->button() == Qt::LeftButton) {
		d->cameraAction = CameraAction::Rotate;
		d->pos1 = Eigen::Vector2f(event->pos().x(), event->pos().y());
	}
	else if (event->button() == Qt::RightButton) {
		d->cameraAction = CameraAction::Move;
		d->pos1 = Eigen::Vector2f(event->pos().x(), event->pos().y());
	}
	
	return XOpenGLWidget::mousePressEvent(event);
}

void MultiPolygonModeCube::mouseMoveEvent(QMouseEvent* event)
{	
	if (d->cameraAction == CameraAction::Rotate) {
		d->pos2 = Eigen::Vector2f(event->pos().x(), event->pos().y());
		d->camera.transformTrackball(d->pos2,d->pos1,mWidth,mHeight,true);
		d->pos1 = d->pos2;
	}
	else if (d->cameraAction == CameraAction::Move) {
		d->pos2 = Eigen::Vector2f(event->pos().x(), event->pos().y());
		d->camera.transformTrackball(d->pos2, d->pos1, mWidth, mHeight, false);
		d->pos1 = d->pos2;
	}

	return XOpenGLWidget::mouseMoveEvent(event);
}

void MultiPolygonModeCube::mouseReleaseEvent(QMouseEvent* event)
{
	//std::cout << std::dec << "鼠标弹起: (" << event->pos().x() << "," << event->pos().y() << ")" << std::endl;
	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton ) {
		d->cameraAction = CameraAction::None;
	}
	
	return XOpenGLWidget::mouseReleaseEvent(event);
}

void MultiPolygonModeCube::wheelEvent(QWheelEvent* event)
{	
	event->angleDelta();
	auto angle = event->angleDelta().y();
	//std::cout << std::dec << "滚轮滚动: " << angle << std::endl;
	double factor = 1;
	if (!std::signbit(angle)) {
		factor = 1.1;
	}
	else {
		factor = 1./1.1;
	}
	d->camera.scale(factor);
	return XOpenGLWidget::wheelEvent(event);
}

void MultiPolygonModeCube::resizeEvent(QResizeEvent* event)
{
	XOpenGLWidget::resizeEvent(event);
	auto width = event->size().width();
	auto height = event->size().height();
	d->camera.m_aspect = (float)width / (float)height;
}
