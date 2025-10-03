#include "XBarItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>

int clamp(int min, int max, int value) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

std::shared_ptr<XFloatArray> computeWidth(std::shared_ptr<XFloatArray> input) {
	//根据坐标计算出每个实例的宽度
	//std::vector<double> width;
	auto width = makeShareDbObject<XFloatArray>();
	auto pointNum = input->getNumOfTuple();
	width->setNumOfTuple(pointNum);
	for (int i = 0; i < pointNum; i++) {

		auto pre = clamp(0, pointNum - 1, i - 1);
		auto next =clamp(0, pointNum - 1, i + 1);

		auto prePos = input->data(pre);
		auto nextPos = input->data(next);
		auto curPos = input->data(i);

		double w = 0;
		if (i == 0) {
			w = std::abs(nextPos[0] - curPos[0]);
		}
		else if ((i+1) == pointNum) {
			w = std::abs(curPos[0] - prePos[0]);
		}
		else {
			w = std::min(std::abs(nextPos[0] - curPos[0]), std::abs(curPos[0] - prePos[0]));
		}
		
		width->setTuple(i,0.5*w);
	}
	return width;
}
XBarItem::XBarItem():XGraphicsItem()
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();
	
	coord->setComponent(3);
	coord->setNumOfTuple(7);

	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, -1, -1, 0);
	coord->setTuple(2, 1, -1, 0);
	coord->setTuple(3, 1, 1, 0);
	coord->setTuple(4, -1, 1, 0);
	coord->setTuple(5, -1, -1, 0);
	coord->setTuple(6, 1, -1, 0);

	m_data = coord;

	auto index = makeShareDbObject<XUIntArray>();
	index->setComponent(3);
	index->setNumOfTuple(2);
	index->setTuple(0, 1, 2, 3);
	index->setTuple(1, 1, 3, 4);

	coord->Modified();
	m_isInstance = true;
	setIsFilled(true);

	this->setIndexArray(index);
}

XBarItem::~XBarItem()
{
}

void XBarItem::updateData()
{
	//顶点数据已经更新
	if (m_data && m_data->GetTimeStamp() > m_UpdateTime) {
		m_vbo_coord->bind();
		m_vbo_coord->allocate(m_data->data(0), m_data->size());
		m_vbo_coord->release();
	}

	{
		if (m_coordArray && m_coordArray->GetTimeStamp() > m_UpdateTime) {
			
			m_instacePos = makeShareDbObject<XFloatArray>();
			m_instacePos->setComponent(16);
			m_instacePos->setNumOfTuple(m_coordArray->getNumOfTuple());
			auto barWidth = computeWidth(m_coordArray);
			auto pbarWidth = barWidth->data(0);
			//根据坐标计算出每个实例的坐标
			for (int i = 0; i < m_coordArray->getNumOfTuple(); i++) {
				Eigen::Affine3f transform = Eigen::Affine3f::Identity();
				auto pos = m_coordArray->data(i);

				//平移到指定位置
				transform.translation() << pos[0], 0.5*pos[1], 0;
				//缩放
				transform.scale(Eigen::Vector3f(0.5* pbarWidth[i], 0.5 * pos[1], 1));
		
				Eigen::Matrix4f m = transform.matrix();
				auto p =m.data();
				m_instacePos->setTuple(i,p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
			}
		}
	}

	{
		if (m_instacePos && m_instacePos->GetTimeStamp() > m_UpdateTime) {

			m_instanceAttrBufffer->bind();

			m_instanceAttrBufffer->allocate(m_instacePos->data(0), m_instacePos->size());

			m_instanceAttrBufffer->release();
		}
	}

	auto m_index = m_indexArray;
	if (m_index && m_index->GetTimeStamp() > m_UpdateTime) {

		m_ebo->bind();
		m_ebo->allocate(m_index->data(0), m_index->size());
		//m_vbo_coord->release();
	}

	////顶点颜色数据已经更新
	auto m_VertexColor = m_colorArray;
	if (m_VertexColor && m_VertexColor->GetTimeStamp() > m_UpdateTime) {
		m_vbo_color->bind();

		m_vbo_color->allocate(m_VertexColor->data(0), m_VertexColor->size());

		m_vbo_color->release();
	}

	//数据已更新，刷新时间戳
	m_UpdateTime.Modified();
}

void XBarItem::drawFill(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	XGraphicsItem::drawFill(shader,m);
}

uint32_t XBarItem::computeNumofVertices()
{
	return m_coordArray->getNumOfTuple()+2;
}

void XBarItem::initResource()
{
	return XGraphicsItem::initResource();
}
