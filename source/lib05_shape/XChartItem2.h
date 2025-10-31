#pragma once
#include "XGraphicsItem.h"

//Õº±Ì¿‡
class XPolyline; 
class XTextItem;

class LIB05_SHAPE_API XChartItem2 :public XGraphicsItem {
public:
	
	XChartItem2();
	virtual ~XChartItem2();

	void setRect(float x, float y, float w, float h);
	void addPolyline(std::shared_ptr<XGraphicsItem> polyline);
	void clearPolylines();

	 void draw(const Eigen::Matrix4f& m) override ;
	//virtual void drawGrid(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m);
	void setBackgroundColor(const myUtilty::Vec4f &color);

	void gridTranslate(float dx, float dy);

	void gridSale(float dx, float dy);

private:
	void createGrid();
	void updateAxisLabel();
protected:
	virtual void updateVboCoord();

	virtual uint32_t computeNumofVertices() override;

	class Internal;
	std::unique_ptr<Internal> d;
};