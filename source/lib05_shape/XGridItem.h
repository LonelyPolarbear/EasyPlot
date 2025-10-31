#pragma once
#include "XGraphicsItem.h"
#include <lib00_utilty/myUtilty.h>

//
class LIB05_SHAPE_API XGridItem :public XGraphicsItem {
public:
enum class GridType {
};
public:
	XGridItem();
	virtual ~XGridItem();

	virtual void draw(const Eigen::Matrix4f& m) override;
	virtual void drawBorder(std::shared_ptr<xshader> border_shader, const Eigen::Matrix4f& m);
	void setIsScreenGrid(bool isScreenGrid);
	bool isScreenGrid() const;
	void setOrigin(const myUtilty::Vec2f& origin);
	const myUtilty::Vec2f& getOrigin() const;
protected:
	virtual uint32_t computeNumofVertices() override;
private:
	bool m_isScreenGrid = true;	//true:屏幕坐标系，false:局部坐标系
	myUtilty::Vec2f mOrigin = myUtilty::Vec2f(0,0);
};