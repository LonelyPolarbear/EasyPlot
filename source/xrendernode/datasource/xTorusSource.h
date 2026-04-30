#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>
#include <vector>


/// <summary>
/// 圆环面
/// </summary>
class XRENDERNODE_API XTorusSource : public XShapeSource {
protected:
	XTorusSource();

	~XTorusSource();
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
	void setStartAngle(float angle);
	void setEndAngle(float angle);
	void setMajorRadius(float radius);
	void setMinorRadius(float radius);

	float getStartAngle() const;
	float getEndAngle() const;
	float getMajorRadius() const;
	float getMinorRadius() const;
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};