#pragma once
#include "../XGraphicsItem.h"
#include <lib00_utilty/XUtilty.h>
#include <Eigen/Eigen>

class XTransformItem;
class xrendernode_API XGridItem :public XGraphicsItem {
public:
enum class GridType {
};
public:
	XGridItem(std::shared_ptr< XGraphicsItem> parent = nullptr );
	virtual ~XGridItem();

	virtual void draw(const Eigen::Matrix4f& m) override;

	virtual void drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m);

	void setIsScreenGrid(bool isScreenGrid);

	bool isScreenGrid() const;

	void setOrigin(const XQ::Vec2f& origin);

	const XQ::Vec2f& getOrigin() const;

	void gridReset();

	void gridTranslate(float dx, float dy);

	void gridSale(float dx, float dy);

	void gridSetSale(float sx, float sy);

    XQ::Vec2f gridGetSale() const;

	void setGridNum(int num);

	int getGridNum() const;

	void setGridSpace(double space);

	double getGridSpace() const;

	void setShowAxis(bool isShowGrid);

	bool isShowAxis() const;

	void addLine(sptr<XGraphicsItem> line);

	Eigen::Affine3f getGridTransform() const;
protected:
	virtual uint32_t computeNumofVertices() override;
	//Eigen::Affine3f m_gridTransform = Eigen::Affine3f::Identity();
private:
	bool m_isScreenGrid = true;	//true:屏幕坐标系，false:局部坐标系
	XQ::Vec2f mOrigin = XQ::Vec2f(0,0);
	bool m_isShowAxis = true;
	//主网格间距
	int  m_gridNum = 5;		//每间隔几个次网格绘制一次主网格
	double  m_gridSpace = 4;
	std::shared_ptr<XTransformItem> m_transformItem;
};