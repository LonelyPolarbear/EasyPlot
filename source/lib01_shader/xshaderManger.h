#pragma once
#include<string>
#include <Eigen/Eigen>
#include <dataBase/XDataBaseObject.h>
#include "xshaderApi.h"
#include "xshader.h"

//着色器管理类，内部预先建立了一些着色器，可以根据需要动态加载其他的着色器
class lib01_shader_API xShaderManger:public XDataBaseObject{
protected:
    xShaderManger();
	~xShaderManger();
public:
	void addShader3D(PrimitveType id, std::shared_ptr<xshader> shader);

	//void addShader2D(PrimitveType id, std::shared_ptr<xshader> shader);

	std::shared_ptr<xshader> getShader3D(PrimitveType id) const;

	//std::shared_ptr<xshader> getShader2D(PrimitveType id) const;

	void setGridShader(std::shared_ptr<xshader> shader);

	void setTextShader(std::shared_ptr<xshader> shader);

	void setNdcShader(std::shared_ptr<xshader> shader);

	void setOutlineShader(std::shared_ptr<xshader> shader);

	std::shared_ptr<xshader> getTextShader() const;

	std::shared_ptr<xshader> getGridShader() const;

	std::shared_ptr<xshader> getNdcShader() const;

	std::shared_ptr<xshader> getOutlineShader() const;

	void InitRenderResource();
protected:
    class Interal;
    std::unique_ptr<Interal> d;
};