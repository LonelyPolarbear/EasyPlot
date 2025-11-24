#pragma once
#include "XGraphicsItem.h"

//图表类
class XPolyline; 
class XTextItem;

class LIB05_SHAPE_API XChartItem2 :public XGraphicsItem {
public:
	
	XChartItem2(std::shared_ptr<XGraphicsItem> parent = nullptr);
	virtual ~XChartItem2();

	void setRect(float x, float y, float w, float h);
	void addPolyline(std::shared_ptr<XGraphicsItem> polyline);
	void clearPolylines();

	 void draw(const Eigen::Matrix4f& m) override ;

	 void fitView();
	
	void setBackgroundColor(const myUtilty::Vec4f &color);

	void setTtitle(const std::wstring& title);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="lastPos">上一次鼠标在场景中的位置</param>
	/// <param name="curPos">当前鼠标在场景中的位置</param>
	void chartTranslate(const myUtilty::Vec2f& lastPos,const myUtilty::Vec2f& curPos);

	void chartSale(float dx, float dy);

	void updateGridFrame();

	void updateChildPosition(const Eigen::Matrix4f&) override;
protected:
	virtual void updateVboCoord();

	virtual uint32_t computeNumofVertices() override;

	class Internal;
	std::unique_ptr<Internal> d;
};