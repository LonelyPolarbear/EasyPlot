#include "xTorusSource.h"
#include <xalgo/XAlgo.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XDataArray.h>

/// <summary>
/// 参数方程含义可以简单理解初始位置在X0Z平面的圆绕着Z旋转得到
/// </summary>
class XTorusSource::Internal {
	public:
	//起始观察角度
	double m_StartAngle = 0.0;		//范围 [0,360)
	//结束观察角度
	double m_EndAngle = 360;		//范围 (0,360]

	double R = 5;
	double r = 1;

	int m_sliceNum = 200;
	int m_circlePointNum =200;
	//如何判断是否闭合,如果m_EndAngle-m_StartAngle 正好一个周期，说明闭合
	//不论闭合与否，我们始终处理当前切片和下一切片即可，当闭合的时候，最后一个切片会和第一个切片重合
	//这样虽然浪费了节点，但是闭合的问题不需要特殊考虑了

	sptr<XFloatArray> generateData() {
		//圆环面，
		auto vec_u = XQ::XAlgo::linspace(m_StartAngle, m_EndAngle, m_sliceNum);
		auto vec_v = XQ::XAlgo::linspace<double>(0, 360, m_circlePointNum);
	
		auto slice_array = makeShareDbObject<XFloatArray>();
		slice_array->setComponent(3);
		//每个切片需要增加中心点的坐标，当圆环不是一个完整周期的时候 需要封口，位置放到每个切片的开头
		auto circlePointNum = m_circlePointNum+1;
		slice_array->setNumOfTuple(m_sliceNum*circlePointNum);

		for (int slice_idx = 0; slice_idx < vec_u.size(); slice_idx++) {
			auto u = vec_u[slice_idx];
			for (int i = 0; i < vec_v.size(); i++) {
				auto v = vec_v[i];
				auto x = (R + r * cos(XQ::Matrix::radian(v))) * cos(XQ::Matrix::radian(u));
				auto y = (R + r * cos(XQ::Matrix::radian(v))) * sin(XQ::Matrix::radian(u));
				auto z = r * sin(XQ::Matrix::radian(v));
				slice_array->setTuple(slice_idx* circlePointNum +i+1, x, y, z);
			}
			//切片的圆心
			{
				auto x = (R + r) * cos(XQ::Matrix::radian(u));
				auto y = (R + r) * sin(XQ::Matrix::radian(u));
				auto z = 0;
				slice_array->setTuple(slice_idx * circlePointNum, x, y, z);
			}
		}
		return slice_array;
	}
};

XTorusSource::XTorusSource():mData(new Internal)
{
	
}

XTorusSource::~XTorusSource()
{
}


void XTorusSource::updateVertexIndexArray()
{
	m_VertexCoord =mData->generateData();
	m_VertexIndexs->Modified();
}

void XTorusSource::setStartAngle(float angle)
{
	mData->m_StartAngle = angle;
	Modified();
}

void XTorusSource::setEndAngle(float angle)
{
	mData->m_EndAngle = angle;
	Modified();
}

void XTorusSource::setMajorRadius(float radius)
{
	mData->R = radius;
	Modified();
}

void XTorusSource::setMinorRadius(float radius)
{
	mData->r = radius;
}

float XTorusSource::getStartAngle() const
{
	return mData->m_StartAngle;
}

float XTorusSource::getEndAngle() const
{
	return mData->m_EndAngle;
}

float XTorusSource::getMajorRadius() const
{
	return mData->R;
}

float XTorusSource::getMinorRadius() const
{
	return mData->r;
}

void XTorusSource::updateVertextCoordArray()
{
	m_VertexCoord->Modified();
}

void XTorusSource::updateVertextNormalArray()
{
}

void XTorusSource::updateVertexColorArray()
{
}

void XTorusSource::updateFaceIndexArray()
{
	//每个切片 有 m_circlePointNum个点 
	//每两层切片之间的三角面片数量 (mData->m_circlePointNum - 1) * 2)
	auto totalTriangleNum = (mData->m_circlePointNum-1)*2*(mData->m_sliceNum-1);

	//每个切片的三角形数量 (mData->m_circlePointNum )，只有第一个和最后一个切片需要特殊处理
	m_FaceIndexs->setNumOfTuple(totalTriangleNum + mData->m_circlePointNum*2);
	int triangleIdx =0;
	auto circlePointNum = mData->m_circlePointNum+1;
	for (int slice_idx = 0; slice_idx < mData->m_sliceNum-1; slice_idx++) {
		//slice_idx			圆心, 0,1,2,3,4,5,6,...,n
		//slice_idx+1		圆心,0,1,2,3,4,5,6,...,n
		auto firstRowStart = slice_idx * circlePointNum;		//+1是为了跳过圆心
		auto secondRowStart = (slice_idx+1) *circlePointNum;
		for (int j = 1; j < circlePointNum-1; j++) {
			m_FaceIndexs->setTuple(triangleIdx++,firstRowStart+j,secondRowStart+j,secondRowStart+j+1 );
			m_FaceIndexs->setTuple(triangleIdx++,firstRowStart+j,secondRowStart+j+1,firstRowStart+j+1);
		}
	}

	//第一个切片的起始位置
	auto firstSliceStart = 0;
	auto lastSliceStart = (mData->m_sliceNum-1) * circlePointNum;
	for (int j = 1; j < circlePointNum-2; j++) {
		m_FaceIndexs->setTuple(triangleIdx++, firstSliceStart, firstSliceStart+j, firstSliceStart + j+1);
	}

	for (int j = 1; j < circlePointNum - 2; j++) {
		m_FaceIndexs->setTuple(triangleIdx++, lastSliceStart, lastSliceStart + j + 1,lastSliceStart + j );
	}

	//第一个切片和最后一个切片需要封口 TODO
	m_FaceIndexs->Modified();
}

void XTorusSource::updateFaceColorArray()
{
	auto torusTriangleNum = (mData->m_circlePointNum - 1) * 2 * (mData->m_sliceNum - 1);

	auto planeTriangleNum = mData->m_circlePointNum * 2;

	auto total = torusTriangleNum + planeTriangleNum;
	m_FaceColor->setNumOfTuple(total);

	for (int i = 0; i < torusTriangleNum; i++) {
		m_FaceColor->setTuple(i, 0, 1, 0, 1);
	}
	for (int i = 0; i < 0.5*planeTriangleNum; i++) {
		m_FaceColor->setTuple(i+ torusTriangleNum, 0, 0, 1, 1);
	}

	for (int i = 0; i < 0.5 * planeTriangleNum; i++) {
		m_FaceColor->setTuple(i + torusTriangleNum+ 0.5 * planeTriangleNum, 0, 0,1, 1);
	}

	m_FaceColor->Modified();
}

void XTorusSource::updateLineIndexArray()
{
	//先底面边 再侧面边
	m_LineIndexs->Modified();
}

void XTorusSource::updateLineColorArray()
{
}
