#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <vector>
#include "xrotateLoftedGeometrySource.h"


/// <summary>
/// 圆锥，圆锥与回转体基类的区别是顶面只有一个点，为了减少不必要顶点数量，重新设计坐标和索引
/// </summary>
class LIB05_SHAPE_API XConeSource : public XRotateLoftedgeometrySource {
protected:
	XConeSource();

	~XConeSource();
	virtual std::vector<XQ::Vec3f> getBottomPoints();
	virtual std::vector<XQ::Vec3f> getTopPoints();
	
	//顶点属性
	void updateVertextCoordArray() override;

	void updateVertextNormalArray()override;

	void updateVertexColorArray()override;

	//面
	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	//线
	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	//点
	void updateVertexIndexArray() override;
public:
	void setNumVertices(unsigned int numVertices);
	void setAngle(double angle);
};