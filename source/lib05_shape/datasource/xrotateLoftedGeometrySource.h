#pragma once
#include "../xshapeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/myUtilty.h>
#include <vector>


class LIB05_SHAPE_API XRotateLoftedgeometrySource : public XShapeSource {
protected:
	XRotateLoftedgeometrySource();

	~XRotateLoftedgeometrySource();

protected:
	void updateVertextCoordArray() override;

	void updateIndexArray()override;

	void updateFaceColorArray()override;

	void updateNormalArray()override;

	void updateVertexColorArray()override;

	void setNumVertices(unsigned int numVertices);

	void setAngle(double angle);

	virtual std::vector<XQ::Vec3f> getBottomPoints() =0;
	virtual std::vector<XQ::Vec3f> getTopPoints()=0;
	protected:
	bool isClosed() const;
	unsigned int m_NumVertices =3;
	double m_height =1;
	double m_radius =1;
	double m_angle =360;		//在指定的角度下绘制m_nNumVertices个点

	//上面一排位置点 形心 点0 点1 点2...
	//下面一排位置点 形心 点0 点1 点2...
};