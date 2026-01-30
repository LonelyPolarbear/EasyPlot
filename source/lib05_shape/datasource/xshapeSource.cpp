#include "xshapeSource.h"
#include <fstream>
#include <Eigen/Eigen>

XShapeSource::XShapeSource()
{
	m_VertexCoord = makeShareDbObject<XFloatArray>(); m_VertexCoord->setComponent(3);
	m_VertexNormal = makeShareDbObject<XFloatArray>(); m_VertexNormal->setComponent(3);
	m_VertexColor = makeShareDbObject<XFloatArray>(); m_VertexCoord->setComponent(3);

	m_FaceColor = makeShareDbObject<XFloatArray>(); m_FaceColor->setComponent(4);
	m_FaceIndexs = makeShareDbObject<XUIntArray>(); m_FaceIndexs->setComponent(3);

	m_LineIndexs = makeShareDbObject<XUIntArray>(); m_LineIndexs->setComponent(2);
	m_LineColor = makeShareDbObject<XFloatArray>(); m_LineColor->setComponent(4);

	m_VertexIndexs = makeShareDbObject<XUIntArray>(); m_VertexIndexs->setComponent(1);

	//更新时间戳，确保数据更新
	Modified();
}

XShapeSource::~XShapeSource()
{
}

XQ::Vec3f XShapeSource::getFaceNormal(uint32_t index)
{
	uint32_t* pIndex = m_FaceIndexs->data(index);
	Eigen::Vector3f v1(m_VertexCoord->data(pIndex[0])[0], m_VertexCoord->data(pIndex[0])[1], m_VertexCoord->data(pIndex[0])[2]);
	Eigen::Vector3f v2(m_VertexCoord->data(pIndex[1])[0], m_VertexCoord->data(pIndex[1])[1], m_VertexCoord->data(pIndex[1])[2]);
	Eigen::Vector3f v3(m_VertexCoord->data(pIndex[2])[0], m_VertexCoord->data(pIndex[2])[1], m_VertexCoord->data(pIndex[2])[2]);
	
	Eigen::Vector3f e1 = v2 - v1;
	Eigen::Vector3f e2 = v3 - v1;
	Eigen::Vector3f normalVec = e1.cross(e2);
	normalVec.normalize();
	auto normal = XQ::Vec3f(normalVec.x(), normalVec.y(), normalVec.z());

	return normal;
}


bool XShapeSource::update()
{
	//更新数据
	if (initSource = false) {
		updateFaceColorArray();
		updateFaceIndexArray();
		updateVertextCoordArray();
		updateVertextNormalArray();
		updateVertexColorArray();
		updateLineIndexArray();
		updateLineColorArray();
		updateVertexIndexArray();
		return false;
	}
	else {
		if (!isNeedUpdate()) {
			return false;
		}

		updateFaceColorArray();
		updateFaceIndexArray();
		updateVertextCoordArray();
		updateVertextNormalArray();
		updateVertexColorArray();
		updateLineIndexArray();
		updateLineColorArray();
		updateVertexIndexArray();

		setHasUpdated();
		return true;
	}
}

XQ::BoundBox XShapeSource::getBoundBox()
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	XQ::Vec3d minBound = XQ::Vec3d(limitMax, limitMax, limitMax);
	XQ::Vec3d maxBound = XQ::Vec3d(limitMin, limitMin, limitMin);

	if (m_VertexCoord->size() == 0)
		return XQ::BoundBox{ minBound.x(), maxBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};

	float* pData = m_VertexCoord->data(0);
	for (int i = 0; i < m_VertexCoord->getNumOfTuple(); i++) {

		XQ::Vec3d pos = XQ::Vec3d(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		minBound = XQ::Vec3d(std::min(minBound.x(), pos.x()), std::min(minBound.y(), pos.y()), std::min(minBound.z(), pos.z()));
		maxBound = XQ::Vec3d(std::max(maxBound.x(), pos.x()), std::max(maxBound.y(), pos.y()), std::max(maxBound.z(), pos.z()));
	}
	return XQ::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};
}

XQ::BoundBox XShapeSource::getBoundBox(const Eigen::Affine3f& mat)
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	XQ::Vec3d minBound = XQ::Vec3d(limitMax, limitMax, limitMax);
	XQ::Vec3d maxBound = XQ::Vec3d(limitMin, limitMin, limitMin);

	if (m_VertexCoord->size() == 0)
		return XQ::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};

	float* pData = m_VertexCoord->data(0);
	for (int i = 0; i < m_VertexCoord->getNumOfTuple(); i++) {

		Eigen::Vector3f pos = Eigen::Vector3f(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		pos = mat * pos;

		minBound = XQ::Vec3d(std::min<double>(minBound.x(), pos.x()), std::min<double>(minBound.y(), pos.y()), std::min<double>(minBound.z(), pos.z()));
		maxBound = XQ::Vec3d(std::max<double>(maxBound.x(), pos.x()), std::max<double>(maxBound.y(), pos.y()), std::max<double>(maxBound.z(), pos.z()));
	}
	return XQ::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};
}

void XShapeSource::writeToFile(const std::string& filename)
{
	m_VertexCoord->getNumOfTuple();

	std::ofstream file(filename);

	file<< m_VertexCoord->getNumOfTuple() << std::endl;

	for (int i = 0; i < m_VertexCoord->getNumOfTuple(); i++) {
		file << m_VertexCoord->data(i)[0] <<" "<< m_VertexCoord->data(i)[1]<<" " <<m_VertexCoord->data(i)[2]<<std::endl;
	}

	file << m_FaceIndexs->getNumOfTuple() << std::endl;

	for (int i = 0; i < m_FaceIndexs->getNumOfTuple(); i++) {
		file << m_FaceIndexs->data(i)[0] << " " << m_FaceIndexs->data(i)[1] << " " << m_FaceIndexs->data(i)[2] << std::endl;
	}
	
	file.close();
}

void XShapeSource::Init()
{
	update();
	initSource = true;
}
