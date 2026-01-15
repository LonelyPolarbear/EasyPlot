#pragma once
#include "xshapeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <Eigen/Eigen>
#include <dataBase/XTimeStamp.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <mutex>
#include <dataBase/XDataArray.h>
#include "XGeometryNode.h"

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

	enum class LIB05_SHAPE_API VAlignment {
		Top,
		Middle,
		Bottom
	};

	enum class LIB05_SHAPE_API HAlignment {
		Left,
		Middle,
		Right
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

//假设有一个正方形 边长为2，原点位于中心，假设有个变换矩阵M，作用在该矩形，现在需要根据变换后的一个位置，反算出原始的点
struct LIB05_SHAPE_API LocalCoordCompute {
	Eigen::Affine3f m_transform;
	LocalCoordCompute(const Eigen::Matrix4f& transform);

	Eigen::Vector3f compute(XGL::Orientation orientation, float disx, float disy) {
		Eigen::Vector3f result = Eigen::Vector3f(0, 0, 0);
		switch (orientation) {
		case XGL::Orientation::left_bottom:
		{
			result = leftBot;
			result += disx * dir_x;
			result += disy * dir_y;
			break;
		}case XGL::Orientation::right_bottom:
		{
			result = rightBot;
			result -= disx * dir_x;
			result += disy * dir_y;
			break;
		}case XGL::Orientation::left_top:
		{
			result = leftTop;
			result += disx * dir_x;
			result -= disy * dir_y;
			break;
		}case XGL::Orientation::right_top:
		{
			result = rightTop;
			result -= disx * dir_x;
			result -= disy * dir_y;
			break;
		}
		default:
			break;
		}
		return m_transform.inverse() * result;
	}

	Eigen::Vector3f compute(XGL::VAlignment alignment, float disx, float disy) {
		//原点在中心, 向右为x轴正方向, 向上为y轴正方向
		Eigen::Vector3f result = Eigen::Vector3f(0, 0, 0);
		switch (alignment) {
		case XGL::VAlignment::Middle: {
			result = center;
			result += disx * dir_x;
			result += disy * dir_y;
			break;
		}
		case XGL::VAlignment::Top: {

			result = 0.5 * (leftTop + rightTop);
			result += disx * dir_x;
			result -= disy * dir_y;
			break;
		}
		case XGL::VAlignment::Bottom: {
			result = 0.5 * (leftBot + rightBot);
			result += disx * dir_x;
			result += disy * dir_y;
			break;
		}
		}

		return m_transform.inverse() * result;
	}

	Eigen::Vector3f compute(XGL::HAlignment alignment, float disx, float disy) {
		//原点在中心, 向右为x轴正方向, 向上为y轴正方向
		Eigen::Vector3f result = Eigen::Vector3f(0, 0, 0);
		switch (alignment) {
		case XGL::HAlignment::Middle: {
			result = center;
			result += disx * dir_x;
			result += disy * dir_y;
			break;
		}
		case XGL::HAlignment::Left: {

			result = 0.5 * (leftBot + leftTop);
			result += disx * dir_x;
			result += disy * dir_y;
			break;
		}
		case XGL::HAlignment::Right: {
			result = 0.5 * (rightTop + rightBot);
			result -= disx * dir_x;
			result += disy * dir_y;
			break;
		}
		}

		return m_transform.inverse() * result;
	}

	Eigen::Vector3f leftBot = Eigen::Vector3f(-1, -1, 0);
	Eigen::Vector3f rightBot = Eigen::Vector3f(1, -1, 0);
	Eigen::Vector3f leftTop = Eigen::Vector3f(-1, 1, 0);
	Eigen::Vector3f rightTop = Eigen::Vector3f(1, 1, 0);
	Eigen::Vector3f center = Eigen::Vector3f(0, 0, 0);
	Eigen::Vector3f dir_x = Eigen::Vector3f::UnitX();
	Eigen::Vector3f dir_y = Eigen::Vector3f::UnitY();

	double scalex = 1;		//X轴缩放比例
	double scaley =1;		//Y轴缩放比例
};

class LIB05_SHAPE_API XGraphicsItem :public XDataBaseObject {
public:
	enum class PenStyle {
		Solid,
		Dash,
		Dot,
		DashDot,
		DashDotDot
	};
	XGraphicsItem(std::shared_ptr<XGraphicsItem> parent);
	virtual ~XGraphicsItem();

	int64_t getID() const;
	sptr< const XGraphicsItem> getChildByID(int64_t id) const;
	sptr< XGraphicsItem> getChildByID(int64_t id);
	void bindSSBO();
	virtual void draw(const Eigen::Matrix4f& m = Eigen::Matrix4f::Identity());
	virtual void pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	virtual void pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
protected:
	virtual void drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m);
	virtual void drawBorderImpl(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m,bool);
	virtual void drawFill(std::shared_ptr<xshader> fill_shader, const Eigen::Matrix4f& m);
	virtual void updateChildPosition(const Eigen::Matrix4f& ){}

	virtual void initResource();
	virtual void initiallize();
public:
	void beginClip(const Eigen::Matrix4f& m);
	void endClip();

	void setTransform(const Eigen::Matrix4f& m);

	Eigen::Affine3f getTransform() const;

	Eigen::Affine3f getParentAccumulateTransform() const;

	sptr<XGraphicsItem> getParent() const;

	void setParent(std::shared_ptr<XGraphicsItem> parent);

	void setCoordArray(std::shared_ptr<XFloatArray> coordArray);
	void setIndexArray(std::shared_ptr<XUIntArray> coordArray);
	void setColorArray(std::shared_ptr<XFloatArray> colorArray);
	void setInstanceArray(std::shared_ptr<XFloatArray> colorArray);
	void setSingleColor(const XQ::Vec4f& color);
	void setPolygonMode(PolygonMode mode);
	void setColorMode(ColorMode mode);
	void setPreSelectColor(const XQ::Vec4f& color);
	void setDrawType(PrimitveType type);

	XQ::Vec4f getSingleColor() const;

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

	XQ::Vec2f getPosition() const;

	XQ::Vec2f getPositionByOrientation() const;

	void scale(float sx, float sy);

	void setScale(float sx, float sy);

	void resetTransform();

	void setVisible(bool visible);

	bool isVisible();

	void setFixedLine(bool fixed);

	void setPenStyle(PenStyle style);

	void setConnectSmoothEnable(bool enable);

	bool isConnectSmoothEnable() const;

	PenStyle getPenStyle() const;

	void setFillColor(const XQ::Vec4f& color);

	XQ::Vec4f getFillColor() const;

	void setIsFilled(bool filled);

	bool isFilled() const;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos">待确认</param>
	/// <returns></returns>
	virtual bool hitTest(const XQ::Vec2f& pos);

	XQ::BoundBox getBoundBox();

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

	void setSceneMatrix(const Eigen::Matrix4f& m);

	Eigen::Matrix4f getSceneMatrix() const;

	virtual uint32_t computeNumofVertices();																		//获取顶点数量
protected:
		//void setIsComposite(bool enable);
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
	bool addAttribute(const std::wstring& key, const std::wstring& value);
	bool removeAttribute(const std::wstring& key);
	std::wstring getAttribute(const std::wstring& key) const;
	std::map<std::wstring, std::wstring> getAttributes() const;
protected:
	//GPU端对象
	std::shared_ptr<XOpenGLVertexArrayObject> m_vao;														//VAO
	std::shared_ptr<XOpenGLBuffer> m_vbo_coord;																//顶点坐标，二维的坐标是屏幕坐标，原点是中心，X轴向右 Y向上
	std::shared_ptr<XOpenGLBuffer> m_vbo_color;																//顶点颜色
	std::shared_ptr<XOpenGLBuffer> m_ebo;																			//用于内部填充
	std::shared_ptr<XOpenGLBuffer> m_ssbo_len;																//每个片元(线段)的长度,主要用于多段线
	std::shared_ptr<XOpenGLBuffer> m_instanceAttrBufffer; //实例化属性的buffer

	XQ::Vec4f m_singleColor = XQ::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	XQ::Vec4f m_fillColor = XQ::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	XQ::Vec4f m_preSelectColor = XQ::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
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

	std::map<std::wstring,std::wstring> m_attributes;													   //属性映射表
};