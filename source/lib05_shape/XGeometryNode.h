#pragma once
#include "xshapeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <Eigen/Eigen>
#include <dataBase/XTimeStamp.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <mutex>
#include <dataBase/XDataArray.h>
#include <glew/glew.h>
#include <lib01_shader/xshader.h>
#include "XRenderNode.h"

#include "mapper/XPolyDataMapper.h"

class xShaderManger;
class XShapeSource;
class LIB05_SHAPE_API XGeometryNode:public XRenderNode3D {
public:
    XGeometryNode();
    virtual ~XGeometryNode();

    /// <summary>
    /// 由外部指定的着色器进行绘制
    /// </summary>
    /// <param name=""></param>
    virtual void draw(std::shared_ptr<xshader>);

	/// <summary>
	/// 对外接口，绘制到屏幕
	/// </summary>
	virtual void draw();

	virtual void drawInstance() {};

	void translate(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setScale(float x, float y, float z);
	void rotate(float angle, XQ::Vec3f dir);

	//按列矩阵形式存储
	float* getMatrix() const;

	void setVisible(bool visible);

	bool isVisible();

	/// <summary>
	/// 平移变换 angle角度，不是弧度
	/// </summary>
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	void scale(float x, float y, float z);

	//对着色器的设置
	void setPolygonMode(PolygonMode mode);
	PolygonMode getPolygonMode() const;

	void setColorMode(ColorMode mode);
	ColorMode getColorMode() const;

	void setSingleColor(XQ::Vec4f color);
	XQ::Vec4f getSingleColor() const;

	void setPreSelectColor(XQ::Vec4f color);
	XQ::Vec4f getPreSelectColor() const;

	bool isSelf(uint64_t id);

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger);

	std::shared_ptr<xShaderManger> getShaderManger() const;

	XQ::BoundBox getBoundBox() const;

	void setPolyDataMapper(sptr<XPolyDataMapper> mapper);
	sptr<XPolyDataMapper> getPolyDataMapper() const;

	sptr<XPolyDataMapper> getOrCreateMapper();

	void setInput(sptr<XShapeSource> input);
private:
	class Internal;
	std::unique_ptr<Internal> d;
 protected:
	ColorMode m_colorMode = ColorMode::SingleColor;
	PrimitveType m_drawType = PrimitveType::triangle;
	PolygonMode m_polygonMode = PolygonMode::fill;
	XQ::Vec4f m_singleColor = XQ::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	XQ::Vec4f m_preSelectColor = XQ::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	bool m_visible = true;

	sptr<xShaderManger> m_shaderManger;
	sptr<XPolyDataMapper> m_polyMapper;
};