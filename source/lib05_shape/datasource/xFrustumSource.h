#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"
#include "XRegularPrimSource.h"


class LIB05_SHAPE_API xfrustumSource : public XRegularPrimSource {
protected:
	xfrustumSource();

	~xfrustumSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
public:
	void setFarPlanePoints(std::vector<XQ::Vec3f> p);	//远平面 底面
	void setNearPlanePoints(std::vector<XQ::Vec3f> p);	//近平面 顶面
	void updateVertextCoordArray() override;
	//void updateLineIndexArray() override;
protected:
	bool hasPoint() const {
		return mFarPlanePoint.size() >0;
	}
protected:
	std::vector<XQ::Vec3f> mFarPlanePoint;  //远平面N个点
	std::vector<XQ::Vec3f> mNearPlanePoint;	//近平面的N个点
};