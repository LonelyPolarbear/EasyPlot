#include "OpenGLWidget2.h"
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

struct OpenGLWidget2::Internal {
	float vertexData[32]{
		//坐标 纹理坐标 颜色值
		-1,1,0,   0,0,    0,1,0,
		-1,-1,0,  0,0,    0,1,0,
		1,-1,0,   0,0,    0,1,0,
		1,1,0,    0,0,    0,1,0,
	};

	unsigned int index[6]{
		0,1,2,
		0,2,3
	};

	//!
	//! global variable
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	xshader shader;
	xcamera camera;
	Eigen::Matrix4f mat_m = Eigen::Matrix4f::Identity();
	CameraAction cameraAction = CameraAction::None;

	//鼠标的位置实时更新
	Eigen::Vector2f pos1 = Eigen::Vector2f(0,0);		//第一个位置
	Eigen::Vector2f pos2 = Eigen::Vector2f(0, 0);		//第二个位置
};

OpenGLWidget2::OpenGLWidget2(QWidget* parent) :XOpenGLWidget(parent),d(std::make_unique<Internal>())
{
	setFocusPolicy(Qt::StrongFocus); // 添加这行
}

OpenGLWidget2::~OpenGLWidget2()
{
}

void OpenGLWidget2::render()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 绘制
	d->shader.use();

	d->camera.projectionMatrix();
	d->camera.getViewMatrix();

	d->mat_m(0, 0) = 0.5;
	d->mat_m(1, 1) = 0.5;

	d->shader.setMat4("m", d->mat_m);
	d->shader.setMat4("v", d->camera.getViewMatrix());
	d->shader.setMat4("p", d->camera.projectionMatrix());

	glBindVertexArray(d->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);


	//交换缓冲
	swapBuffers();
	glUseProgram(0);
	doneCurrent();
}

void OpenGLWidget2::initGLResource()
{
	d->shader = xshader(myUtilty::ShareVar::instance().currentExeDir + "/01triangle/" + "triangle.vs", myUtilty::ShareVar::instance().currentExeDir + "/01triangle/" + "triangle.fs");
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
	glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(float), d->vertexData, GL_STATIC_DRAW);

	//!
	//! allocate data to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), d->index, GL_STATIC_DRAW);

	//!
	//! set vao attribute
	glBindVertexArray(d->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	double boundBox[6] {-1,-1,-1,1,1,1};
	d->camera.resetCamera(boundBox);
}

void OpenGLWidget2::mousePressEvent(QMouseEvent* event)
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

void OpenGLWidget2::mouseMoveEvent(QMouseEvent* event)
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

void OpenGLWidget2::mouseReleaseEvent(QMouseEvent* event)
{
	//std::cout << std::dec << "鼠标弹起: (" << event->pos().x() << "," << event->pos().y() << ")" << std::endl;
	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton ) {
		d->cameraAction = CameraAction::None;
	}
	
	return XOpenGLWidget::mouseReleaseEvent(event);
}

void OpenGLWidget2::wheelEvent(QWheelEvent* event)
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

void OpenGLWidget2::resizeEvent(QResizeEvent* event)
{
	XOpenGLWidget::resizeEvent(event);
	auto width = event->size().width();
	auto height = event->size().height();
	d->camera.m_aspect = (float)width / (float)height;
}
