#pragma once
#include "xshapeApi.h"
#include <dataBase/dataobject.h>
#include <Eigen/Eigen>
#include <dataBase/XTimeStamp.h>
#include <lib00_utilty/myUtilty.h>
#include <mutex>
#include <dataBase/XDataArray.h>
#include <glew/glew.h>
#include <lib01_shader/xshader.h>

class XOpenGLVertexArrayObject;
class XOpenGLBuffer;
class XShapeSource;
class xShaderManger;

enum class LIB05_SHAPE_API  ColorMode {
	SingleColor =1,
	VertexColor =2,
	FaceColor =3,
	textureColor =4,
	SelectTest=5						//用于拾取判断
};

enum class LIB05_SHAPE_API  PolygonMode {
	point =1,
	line =2,
	fill =3,
	line_fill =4
};

/// <summary>
/// XShape是所有自定义图形的基类，它提供了一些基本的接口，包括初始化资源、绘制、修改标志位等。
/// </summary>
class LIB05_SHAPE_API XShape:public DataBaseObject {
public:
    XShape();
    virtual ~XShape();

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

    virtual void initResource();

	virtual void initiallize();

	int64_t getID() const;

	void bindSSBO();

	void translate(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setScale(float x, float y, float z);
	void rotate(float angle, myUtilty::Vec3f dir);

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
	void setSingleColor(myUtilty::Vec4f color);
	myUtilty::Vec4f getSingleColor() const;

	void setPreSelectColor(myUtilty::Vec4f color);
	myUtilty::Vec4f getPreSelectColor() const;

	myUtilty::Vec4f computeSelectTestColor();

	static uint64_t colorToUInt(myUtilty::Vec4f color);

	bool isSelf(uint64_t id);

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger);

	std::shared_ptr<xShaderManger> getShaderManger() const;

	//CPU端数据输入
	std::shared_ptr<XFloatArray> getCoordAarray() const;
	std::shared_ptr<XFloatArray> getNormalArray() const;
	std::shared_ptr<XFloatArray> getVertexColorArray() const;
	std::shared_ptr<XFloatArray> getFaceColorArray() const;
	std::shared_ptr<XUIntArray> getIndexsArray() const;

	//GPU端数据更新
	virtual void updateData();

	void setInput(std::shared_ptr<XShapeSource> input);

	std::shared_ptr<XShapeSource> getInput() const {
		return m_Input;
	}

	myUtilty::BoundBox getBoundBox() const;
private:
	class Internal;
	std::unique_ptr<Internal> d;
 protected:
	//GPU端对象
	sptr<XOpenGLVertexArrayObject> m_vao;					//VAO
	sptr<XOpenGLBuffer> m_vbo_coord;							//顶点坐标，必须有
	sptr<XOpenGLBuffer> m_vbo_normal;						//法线
	sptr<XOpenGLBuffer> m_vbo_color;							//颜色
	sptr<XOpenGLBuffer> m_ssbo_color;							//每个片元的颜色
	sptr<XOpenGLBuffer> m_ebo;										//EBO
	
	//CPU端数据
	sptr<XShapeSource> m_Input;;

	//着色器相关设置
	ColorMode m_colorMode = ColorMode::FaceColor;
	PrimitveType m_drawType = PrimitveType::triangle;
	PolygonMode m_polygonMode = PolygonMode::line_fill;
	myUtilty::Vec4f m_singleColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	myUtilty::Vec4f m_preSelectColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	myUtilty::Vec4f m_selectTestColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

	//同步CPU和GPU端数据的时间戳
	XTimeStamp m_UpdateTime;
	bool m_visible = true;
	sptr<xShaderManger> m_shaderManger;
};
