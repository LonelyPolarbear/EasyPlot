#include "OpenGLWidget1.h"
#include <glew/glew.h>
#include <windows.h>
#include <Eigen/Eigen>
#include <lib01_shader/xshader.h>
#include <lib00_utilty/myUtilty.h>

float vertexData[]{
	//坐标 纹理坐标 颜色值
	-1,1,0,   0,0,    0,1,0,
	-1,-1,0,  0,0,    0,1,0,
	1,-1,0,   0,0,    0,1,0,
	1,1,0,    0,0,    0,1,0,
};

unsigned int index[]{
	0,1,2,
	0,2,3
};

//!
//! global variable
GLuint VAO;
GLuint VBO;
GLuint EBO;
xshader shader;
Eigen::Matrix4f mat_m = Eigen::Matrix4f::Identity();
Eigen::Matrix4f mat_v = Eigen::Matrix4f::Identity();
Eigen::Matrix4f mat_p = Eigen::Matrix4f::Identity();

OpenGLWidget1::OpenGLWidget1(QWidget* parent) :XOpenGLWidget(parent)
{
	
}

void OpenGLWidget1::render()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 绘制
	shader.use();


	mat_m(0, 0) = 0.5;
	mat_m(1, 1) = 0.5;

	shader.setMat4("m", mat_m);
	shader.setMat4("v", mat_v);
	shader.setMat4("p", mat_p);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);


	//交换缓冲
	swapBuffers();
	glUseProgram(0);
	doneCurrent();
}

void OpenGLWidget1::initGLResource()
{
	shader = xshader(myUtilty::ShareVar::instance().currentExeDir + "/01triangle/" + "triangle.vs", myUtilty::ShareVar::instance().currentExeDir + "/01triangle/" + "triangle.fs");
	//!
	//! create VAO VBO EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//!
	//! bind vbo ebo to vao
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//!
	//! allocate data to vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(float), vertexData, GL_STATIC_DRAW);

	//!
	//! allocate data to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), index, GL_STATIC_DRAW);

	//!
	//! set vao attribute
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
