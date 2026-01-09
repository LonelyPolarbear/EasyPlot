#include "xchamferCubeSource.h"

xchamferCubeSource::xchamferCubeSource()
{
}

xchamferCubeSource::~xchamferCubeSource()
{
}

void xchamferCubeSource::updateVertextCoordArray()
{
	//更新顶点
	//前面的四个坐标点
	m_coord->setNumOfTuple(24);
	float radius = 0.15f;
	float halfsideLen = 1.0f;
	float side = halfsideLen - radius;

	//前面的四个坐标点,每个面使用右手定则，大拇指指向立方体外部，第一个点是左下角
	/*
	3-----2
	|       |
	0-----1
	*/
	//z+
	m_coord->setTuple(0, -side, -side, 1);
	m_coord->setTuple(1, side, -side, 1);
	m_coord->setTuple(2, side, side, 1);
	m_coord->setTuple(3, -side, side, 1);

	//z-
	/*
	6----- 7
	|       |
	5----- 4
	*/
	m_coord->setTuple(4, side, -side, -1);
	m_coord->setTuple(5, -side, -side, -1);
	m_coord->setTuple(6, -side, side, -1);
	m_coord->setTuple(7, side, side, -1);

	//y+
	/*
	11----- 10
	|       |
	8----- 9
	*/
	m_coord->setTuple(8, -side, 1, side);
	m_coord->setTuple(9, side, 1, side);
	m_coord->setTuple(10, side, 1, -side);
	m_coord->setTuple(11, -side, 1, -side);

	//y-
	/*
	12----- 13
	|       |
	15----- 14
	*/
	m_coord->setTuple(12, -side, -1, -side);
	m_coord->setTuple(13, side, -1, -side);
	m_coord->setTuple(14, side, -1, side);
	m_coord->setTuple(15, -side, -1, side);

	//x+
	/*
	19----- 18
	|       |
	16----- 17
	*/
	m_coord->setTuple(16, 1, -side, side);
	m_coord->setTuple(17, 1, -side, -side);
	m_coord->setTuple(18, 1, side, -side);
	m_coord->setTuple(19, 1, side, side);

	//x-
	/*
	23----- 22
	|       |
	20----- 21
	*/
	m_coord->setTuple(20, -1, -side, -side);
	m_coord->setTuple(21, -1, -side, side);
	m_coord->setTuple(22, -1, side, side);
	m_coord->setTuple(23, -1, side, -side);



	m_coord->Modified();
}

void xchamferCubeSource::updateIndexArray()
{
	m_indexs->setNumOfTuple(44);
	//前
	m_indexs->setTuple(0, 0, 1, 2);
	m_indexs->setTuple(1, 0, 2, 3);

	//后
	m_indexs->setTuple(2, 4, 5,6);
	m_indexs->setTuple(3, 4,6, 7);

	//上
	m_indexs->setTuple(4, 8,9,10);
	m_indexs->setTuple(5, 8,10,11);

	//下
	m_indexs->setTuple(6, 12, 13, 14);
	m_indexs->setTuple(7, 12, 14, 15);

	//左
	m_indexs->setTuple(8, 16, 17, 18);
	m_indexs->setTuple(9, 16, 18, 19);

	//右
	m_indexs->setTuple(10, 20, 21, 22);
	m_indexs->setTuple(11, 20, 22, 23);

	//Z+和Y+的四边形
	m_indexs->setTuple(12, 3, 2, 9);
	m_indexs->setTuple(13, 3, 9, 8);

	//Z+和Y-的四边形
	m_indexs->setTuple(14, 15, 14, 1);
	m_indexs->setTuple(15, 15, 1, 0);

	//Z+和X+的四边形
	m_indexs->setTuple(16, 1, 16, 19);
	m_indexs->setTuple(17, 1, 19, 2);

	//Z+和X-的四边形
	m_indexs->setTuple(18, 0, 3, 22);
	m_indexs->setTuple(19, 0,22, 21);

	//Z-和Y+的四边形
	m_indexs->setTuple(20, 11, 10, 7);
	m_indexs->setTuple(21, 11, 7,6);

	//Z-和Y-的四边形
	m_indexs->setTuple(22, 13, 12, 5);
	m_indexs->setTuple(23, 13, 5, 4);

	//Z-和X+的四边形
	m_indexs->setTuple(24, 18, 17, 4);
	m_indexs->setTuple(25, 18, 4, 7);

	//Z-和X-的四边形
	m_indexs->setTuple(26, 5, 20,23);
	m_indexs->setTuple(27, 5, 23, 6);

	//Y+和X+的四边形
	m_indexs->setTuple(28, 9, 19, 18);
	m_indexs->setTuple(29, 9,18,10);

	//Y+和X-的四边形
	m_indexs->setTuple(30, 11, 23, 22);
	m_indexs->setTuple(31, 11, 22, 8);

	//Y-和X+的四边形
	m_indexs->setTuple(32, 16, 14, 13);
	m_indexs->setTuple(33, 16, 13, 17);

	//Y-和X-的四边形
	m_indexs->setTuple(34, 20,12, 15);
	m_indexs->setTuple(35, 20, 15, 21);

	//z+ y+ x-的三角形
	m_indexs->setTuple(36, 8, 22, 3);

	//z+ y+ x+的三角形
	m_indexs->setTuple(37, 9, 2, 19);

	//z+ y- x+的三角形
	m_indexs->setTuple(38, 1, 14, 16);

	//z+ y- x-的三角形
	m_indexs->setTuple(39, 0, 21, 15);

	//z- y+ x-的三角形
	m_indexs->setTuple(40, 6, 23, 11);

	//z- y+ x+的三角形
	m_indexs->setTuple(41, 10, 18, 7);

	//z- y- x+的三角形
	m_indexs->setTuple(42, 4, 17, 13);

	//z- y- x-的三角形
	m_indexs->setTuple(43, 5, 12,20);

	m_indexs->Modified();
}

void xchamferCubeSource::updateFaceColorArray()
{
	m_FaceColor->setNumOfTuple(44);
	//前后
	m_FaceColor->setTuple(0, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(1, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(2, 0.0f, 0.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(3, 0.0f, 0.0f, 1.0f, 1.0f);

	//上下
	m_FaceColor->setTuple(4, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(5, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(6, 0.0f, 1.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(7, 0.0f, 1.0f, 0.0f, 1.0f);

	//左右
	m_FaceColor->setTuple(8, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(9, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(10, 1.0f, 0.0f, 0.0f, 1.0f);
	m_FaceColor->setTuple(11, 1.0f, 0.0f, 0.0f, 1.0f);

	//Z+和Y+的四边形
	m_FaceColor->setTuple(12, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(13, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z+和Y-的四边形
	m_FaceColor->setTuple(14, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(15, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z+和X+的四边形
	m_FaceColor->setTuple(16, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(17, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z+和X-的四边形
	m_FaceColor->setTuple(18, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(19, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z-和Y+的四边形
	m_FaceColor->setTuple(20, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(21, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z-和Y-的四边形
	m_FaceColor->setTuple(22, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(23, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z-和x+的四边形
	m_FaceColor->setTuple(24, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(25, 0.0f, 1.0f, 1.0f, 1.0f);

	//Z-和x-的四边形
	m_FaceColor->setTuple(26, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(27, 0.0f, 1.0f, 1.0f, 1.0f);

	//Y+和x+的四边形
	m_FaceColor->setTuple(28, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(29, 0.0f, 1.0f, 1.0f, 1.0f);

	//Y+和x-的四边形
	m_FaceColor->setTuple(30, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(31, 0.0f, 1.0f, 1.0f, 1.0f);

	//Y-和x+的四边形
	m_FaceColor->setTuple(32, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(33, 0.0f, 1.0f, 1.0f, 1.0f);

	//Y-和x-的四边形
	m_FaceColor->setTuple(34, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(35, 0.0f, 1.0f, 1.0f, 1.0f);

	//z+ y+ x-的三角形
	m_FaceColor->setTuple(36, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(37, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(38, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(39, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(40, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(41, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(42, 0.0f, 1.0f, 1.0f, 1.0f);
	m_FaceColor->setTuple(43, 0.0f, 1.0f, 1.0f, 1.0f);

	m_FaceColor->Modified();
}

void xchamferCubeSource::updateNormalArray()
{
	m_normal->Modified();
}

void xchamferCubeSource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}