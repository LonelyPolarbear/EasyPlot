#pragma once
#include "../xrendernodeApi.h"
#include "xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <vector>

/// <summary>
/// 回转体
/// </summary>
class xrendernode_API XRotateLoftedgeometrySource : public XShapeSource {
protected:
	XRotateLoftedgeometrySource();

	~XRotateLoftedgeometrySource();
public:
	void Init() override;
protected:
	void updateVertextCoordArray() override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override;

	void updateVertextNormalArray()override;

	void updateVertexColorArray()override;

	void updateLineIndexArray() override;

	void updateLineColorArray() override;

	void updateVertexIndexArray() override;

	void setNumVertices(unsigned int numVertices);

	void setAngle(double angle);

	//点的环绕顺序遵循原则：从上顶面 上方朝着下底面看，上顶面和下底面全部逆时针旋转
	virtual std::vector<XQ::Vec3f> getBottomPoints() =0;		//下地面 顺时针旋转
	virtual std::vector<XQ::Vec3f> getTopPoints()=0;				//上顶面 逆时针旋转
	protected:
	bool isClosed() const;
	unsigned int m_NumVertices =3;
	double m_height =1;
	double m_radius =1;
	double m_angle =360;		//在指定的角度下绘制m_nNumVertices个点

	//上面一排位置点 形心 点0 点1 点2...
	//下面一排位置点 形心 点0 点1 点2...
};