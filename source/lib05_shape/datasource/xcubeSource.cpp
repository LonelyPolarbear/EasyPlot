#include "xcubeSource.h"

XCubeSource::XCubeSource()
{
}

XCubeSource::~XCubeSource()
{
}

void XCubeSource::updateVertextCoordArray()
{
	//更新顶点
	//前面的四个坐标点
	m_coord->setNumOfTuple(8);
	m_coord->setTuple(0, -1.0f, -1.0f, 1.0f);
	m_coord->setTuple(1, -1.0f, 1.0f, 1.0f);
	m_coord->setTuple(2, 1.0f, 1.0f, 1.0f);
	m_coord->setTuple(3, 1.0f, -1.0f, 1.0f);

	//后面的四个坐标点
	m_coord->setTuple(4, -1.0f, -1.0f, -1.0f);
	m_coord->setTuple(5, 1.0f, -1.0f, -1.0f);
	m_coord->setTuple(6, 1.0f, 1.0f, -1.0f);
	m_coord->setTuple(7, -1.0f, 1.0f, -1.0f);

	m_coord->Modified();
}

void XCubeSource::updateIndexArray()
{
	m_indexs->setNumOfTuple(12);
	//前
	m_indexs->setTuple(0, 0, 3, 2);
	m_indexs->setTuple(1, 0, 2, 1);

	//后
	m_indexs->setTuple(2, 5, 4, 7);
	m_indexs->setTuple(3, 5, 7, 6);

	//左
	m_indexs->setTuple(4, 4, 0, 1);
	m_indexs->setTuple(5, 4, 1, 7);

	//右
	m_indexs->setTuple(6, 3, 5, 6);
	m_indexs->setTuple(7, 3, 6, 2);

	//上
	m_indexs->setTuple(8, 1, 2, 6);
	m_indexs->setTuple(9, 1, 6, 7);

	//下
	m_indexs->setTuple(10, 4, 5, 3);
	m_indexs->setTuple(11, 4, 3, 0);

	m_indexs->Modified();
}

void XCubeSource::updateFaceColorArray()
{
	m_FaceColor->setNumOfTuple(12);
	//前后
	m_FaceColor->setTuple(0, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(1, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(3, 0.0f, 0.0f, 1.0f, 1.0f);

	//左右
	m_FaceColor->setTuple(4, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(5, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(6, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(7, 1.0f, 0.0f, 0.0f, 1.0f);

	//上下
	m_FaceColor->setTuple(8, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(9, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(10, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(11, 0.0f, 1.0f, 0.0f, 1.0f);

	m_FaceColor->Modified();
}

void XCubeSource::updateNormalArray()
{
	m_normal->Modified();
}

void XCubeSource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}