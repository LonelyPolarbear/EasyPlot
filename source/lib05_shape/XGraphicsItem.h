#pragma once
#include "xshapeApi.h"
#include <dataBase/dataobject.h>
#include <Eigen/Eigen>
#include <dataBase/XTimeStamp.h>
#include <lib00_utilty/myUtilty.h>
#include <mutex>
#include <dataBase/XDataArray.h>
#include "xshape.h"

class xshader;
class xcamera;
class XOpenGLVertexArrayObject;
class XOpenGLBuffer;
class xShaderManger;

namespace XGL {
	enum class LIB05_SHAPE_API Orientation {
		left_top,
		left_bottom,
		right_top,
		right_bottom
	};

	enum class LIB05_SHAPE_API Layout {
		horizontal,
		vertical
	};

	enum class LIB05_SHAPE_API PositionType {
		local_complete,					//所有的点都在本地坐标系中，包括中心点
		local_center,						//只有中心点在本地坐标系中
		sceneScreen_complete,		//场景屏幕坐标系，同时意味着固定大小
		sceneScreen_center,			//场景屏幕坐标系，只有中心点在屏幕坐标系中
	};
};


class LIB05_SHAPE_API XGraphicsItem :public DataBaseObject {
public:
	enum class PenStyle {
		Solid,
		Dash,
		Dot,
		DashDot,
		DashDotDot
	};
	XGraphicsItem(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XGraphicsItem();

	int64_t getID() const;
	void bindSSBO();
	virtual void draw(const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());
	virtual void pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual void pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
protected:
	virtual void drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m);
	virtual void drawBorderImpl(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m,bool);
	virtual void drawFill(std::shared_ptr<xshader> fill_shader, const Eigen::Matrix4f& m);

	void beginClip(const Eigen::Matrix4f& m);
	void endClip();
	virtual void initResource();
	virtual void initiallize();
public:
	
	Eigen::Affine3f getTransform() const;

	void setCoordArray(std::shared_ptr<XFloatArray> coordArray);
	void setIndexArray(std::shared_ptr<XUIntArray> coordArray);
	void setColorArray(std::shared_ptr<XFloatArray> colorArray);
	void setInstanceArray(std::shared_ptr<XFloatArray> colorArray);
	void setSingleColor(const myUtilty::Vec4f& color);
	void setPolygonMode(PolygonMode mode);
	void setColorMode(ColorMode mode);
	void setPreSelectColor(const myUtilty::Vec4f& color);
	void setDrawType(PrimitveType type);
	
	bool isComposite() const;

	myUtilty::Vec4f getSingleColor() const;

	PrimitveType getDrawType() const;

	void setLineWidth(uint32_t width);

	void setShaderManger(std::shared_ptr<xShaderManger> shaderManger);

	std::shared_ptr<xShaderManger> getShaderManger() const;

	uint32_t getLineWidth() const;

	std::shared_ptr<XFloatArray> getCoordArray() const;

	void rotate(float angle);

	void translate(float dx, float dy);

	void setPosition(float x, float y);

	void setPositionType(XGL::PositionType type);

	XGL::PositionType getPositionType() const;

	void setOrientation(XGL::Orientation orientation);
	XGL::Orientation getOrientation() const;

	myUtilty::Vec2f getPosition() const;

	myUtilty::Vec2f getPositionByOrientation() const;

	void scale(float sx, float sy);

	void resetTransform();

	void setVisible(bool visible);

	bool isVisible();

	void setFixedLine(bool fixed);

	void setPenStyle(PenStyle style);

	void setConnectSmoothEnable(bool enable);

	bool isConnectSmoothEnable() const;

	PenStyle getPenStyle() const;

	void setFillColor(const myUtilty::Vec4f& color);

	myUtilty::Vec4f getFillColor() const;

	void setIsFilled(bool filled);

	bool isFilled() const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos">待确认</param>
	/// <returns></returns>
	virtual bool hitTest(const myUtilty::Vec2f& pos);

	myUtilty::BoundBox getBoundBox();

	float* getMatrix() const;

	ColorMode m_colorMode = ColorMode::SingleColor;
	PolygonMode m_polygonMode = PolygonMode::line;
	PrimitveType m_drawType = PrimitveType::line_strip_adjacency;

	PenStyle m_penStyle = PenStyle::Solid;
	bool m_clipEnable = false;																									//是否开启裁剪
	bool m_isShowGrid = false;																								//是否显示网格
	bool m_IsVisible = true;
	bool m_IsClosed = true;																										//如果是闭合曲线则允许填充
	bool m_IsFilled = false;																										//是否填充
	bool m_IsFixedLine = false;																								//是否固定线宽
	bool m_connectSmoothEnable = true;																				//是否连接曲线平滑
	bool m_hasSSBOLen = false;
	bool m_isInstance = false;
	uint32_t m_lineWidth = 1;																									//线宽
	virtual void updateData();																									//当m_coordArray和m_colorArray更改，更新缓冲
	virtual void updateVboCoord();
	virtual void updateVboColor();
	virtual void updateVboEbo();
	virtual void updateVboInstance();

	virtual uint32_t computeNumofVertices();																		//获取顶点数量
protected:
		void setIsComposite(bool enable);
private:
	class Internal;
	std::unique_ptr<Internal> d;

	public:
	void addChildItem(std::shared_ptr<XGraphicsItem> item);
	void addChildItem(std::vector<std::shared_ptr<XGraphicsItem>> items);
	void removeChildItem(std::shared_ptr<XGraphicsItem> item);
	void clearChildItems();
	void setParentItem(std::shared_ptr<XGraphicsItem> item);
	std::shared_ptr<XGraphicsItem> getParentItem() const;
protected:
	//GPU端对象
	std::shared_ptr<XOpenGLVertexArrayObject> m_vao;														//VAO
	std::shared_ptr<XOpenGLBuffer> m_vbo_coord;																//顶点坐标，二维的坐标是屏幕坐标，原点是中心，X轴向右 Y向上
	std::shared_ptr<XOpenGLBuffer> m_vbo_color;																//顶点颜色
	std::shared_ptr<XOpenGLBuffer> m_ebo;																			//用于内部填充
	std::shared_ptr<XOpenGLBuffer> m_ssbo_len;																//每个片元(线段)的长度,主要用于多段线
	std::shared_ptr<XOpenGLBuffer> m_instanceAttrBufffer; //实例化属性的buffer

	myUtilty::Vec4f m_singleColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	myUtilty::Vec4f m_fillColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	myUtilty::Vec4f m_preSelectColor = myUtilty::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	XGL::PositionType m_positionType = XGL::PositionType::local_complete;
	XGL::Orientation m_orientation = XGL::Orientation::left_bottom;

	std::shared_ptr<XFloatArray> m_coordArray;																	//顶点坐标数组
	std::shared_ptr<XFloatArray> m_colorArray;																	//顶点颜色数组
	std::shared_ptr<XUIntArray> m_indexArray;																	//顶点索引数组
	std::shared_ptr<XFloatArray> m_instacePos;																	//实例化位置

	//同步CPU和GPU端数据的时间戳
	XTimeStamp m_UpdateTime;

	std::shared_ptr<xShaderManger> m_shaderManger;

	std::shared_ptr<XGraphicsItem> m_parentItem;															//父图元
	std::vector<std::shared_ptr<XGraphicsItem>> m_childItems;									//子图元

	bool mIsComposite = false;
};