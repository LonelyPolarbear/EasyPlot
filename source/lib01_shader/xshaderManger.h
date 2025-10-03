#pragma once
#include<string>
#include <Eigen/Eigen>
#include <dataBase/dataobject.h>
#include "xshaderApi.h"
#include "xshader.h"

//着色器管理类，内部预先建立了一些着色器，可以根据需要动态加载其他的着色器
class lib01_shader_API xShaderManger:public DataBaseObject{
protected:
    xShaderManger();
	~xShaderManger();
public:
	void addShader3D(PrimitveType id, std::shared_ptr<xshader> shader);

	void addShader2D(PrimitveType id, std::shared_ptr<xshader> shader);

	std::shared_ptr<xshader> getShader3D(PrimitveType id) const;

	std::shared_ptr<xshader> getShader2D(PrimitveType id) const;

	void setFillShader(std::shared_ptr<xshader> shader);

	void setGridShader3D(std::shared_ptr<xshader> shader);

	void setGridShader2D(std::shared_ptr<xshader> shader);

	void setPickShader3D(std::shared_ptr<xshader> shader);

	void setPickShader2D(std::shared_ptr<xshader> shader);

	void setPickFillShader2D(std::shared_ptr<xshader> shader);

	void setTextShader(std::shared_ptr<xshader> shader);

	std::shared_ptr<xshader> getTextShader() const;

	std::shared_ptr<xshader> getFillShader() const ;

	std::shared_ptr<xshader> getGridShader3D() const;

	std::shared_ptr<xshader> getGridShader2D() const;

	std::shared_ptr<xshader> getPickShader3D() const;

	std::shared_ptr<xshader> getPickShader2D() const;

	std::shared_ptr<xshader> getPickFillShader2D() const;

	void initGLResource();
protected:
    class Interal;
    std::unique_ptr<Interal> d;
};