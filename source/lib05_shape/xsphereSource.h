
#pragma once
#include "xshapeApi.h"
#include "xshapeSource.h"

class LIB05_SHAPE_API XSphereSource : public XShapeSource
{
public:
	enum Direaction {
		horizontal,
		vertical
	};
	XSphereSource();

	virtual ~XSphereSource();

	void setRadius(double r);

	void setDirection(Direaction d);

	unsigned int getDirectionTriangleNum() const;

	void setSNRange(double start, double end);

	void setEWRange(double start, double end);

	void setSNNum(int num);

	void setEWNum(int num);

	unsigned int gteNumberOfTriangle() const;

	unsigned int getSurfaceNumberOfTriangle() const;

	unsigned int getEWNumberOfTriangle() const;

	unsigned int getSNNumberOfTriangle() const;

	unsigned int getSurfaceNumberPoints() const;

	unsigned int getNumofSNRestPoint() const;

	unsigned int getNumofEWRestPoint() const;
public:

	void updateVertextCoordArray() override;

	void updateIndexArray()override;

	void updateFaceColorArray()override;

	void updateNormalArray()override;

	void updateVertexColorArray()override;


	//圆柱的参数方程
	//极径与Z轴的夹角为phi[0-180]，半径为r，高度为h(理解为维度)
	//极径在XOY平面的投影与X轴的夹角为θ[-180-180](理解为经度)
private:
	class Internal;
	std::unique_ptr<Internal> d;
};

