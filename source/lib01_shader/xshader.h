#ifndef XSHADER_HPP
#define XSHADER_HPP

#include<string>
#include <Eigen/Eigen>
#include <dataBase/dataobject.h>
#include "xshaderApi.h"
#include "XBaseShader.h"

//着色器类


class lib01_shader_API xshader:public XBaseShader {
 public:
    xshader() = default;

    ~xshader();

	xshader(const std::string& vertexPath, const std::string& fragmentPath, bool isPath = true);

	xshader(std::map<ShaderType,std::string> shaderPathsOrCode, bool isPath = true);
public:

	void setModelMatrix(float* data);

	void setModelSelfMatrix(float* data);

    void setViewMatrix(float* data);

    void setViewMatrix(const Eigen::Matrix4f& mat);

	void setProjectionMatrix(float* data);

	void setProjectionMatrix(const Eigen::Matrix4f& mat);

	void setColorMode(int mode);

	void setPolygonMode(int mode);

	void setSingleColor(float r, float g, float b, float a);

    void setFillColor(float r, float g, float b, float a);

	void setSelectTestColor(float r, float g, float b, float a);

    void setIs3D(bool is3D);

	void setScreeSize(int w, int h);

	void setRealTimeMousePos(int w, int h);

    void setPreSelectColor(float r, float g, float b, float a);

    void setObjectID(uint32_t t);

    void setLineWidth(uint32_t width);

    void setfixedLine(bool isFixed);

    void setPenStyle(int style);

    void setPositionType(int type);

    void setOrientation(int type);

    void setConenctSmoothEnable(bool enable);

    void setIsInstanceDarw(bool isInstance);

    void textureBinding(const std::string& name, int bindingPoint);

	void uboBinding(const std::string& name, int bindingPoint);

	void ssboBinding(const std::string& name, int bindingPoint);
};
#endif