#include "xshapeSource.h"
#include <fstream>
#include <Eigen/Eigen>

XShapeSource::XShapeSource()
{
	m_coord = makeShareDbObject<XFloatArray>(); m_coord->setComponent(3);
	m_normal = makeShareDbObject<XFloatArray>(); m_normal->setComponent(3);
	m_VertexColor = makeShareDbObject<XFloatArray>(); m_coord->setComponent(3);
	m_FaceColor = makeShareDbObject<XFloatArray>(); m_FaceColor->setComponent(4);
	m_indexs = makeShareDbObject<XUIntArray>(); m_indexs->setComponent(3);

	//更新时间戳，确保数据更新
	Modified();
}

XShapeSource::~XShapeSource()
{
}

myUtilty::Vec3f XShapeSource::getFaceNormal(uint32_t index)
{
	uint32_t* pIndex = m_indexs->data(index);
	Eigen::Vector3f v1(m_coord->data(pIndex[0])[0], m_coord->data(pIndex[0])[1], m_coord->data(pIndex[0])[2]);
	Eigen::Vector3f v2(m_coord->data(pIndex[1])[0], m_coord->data(pIndex[1])[1], m_coord->data(pIndex[1])[2]);
	Eigen::Vector3f v3(m_coord->data(pIndex[2])[0], m_coord->data(pIndex[2])[1], m_coord->data(pIndex[2])[2]);
	
	Eigen::Vector3f e1 = v2 - v1;
	Eigen::Vector3f e2 = v3 - v1;
	Eigen::Vector3f normalVec = e1.cross(e2);
	normalVec.normalize();
	auto normal = myUtilty::Vec3f(normalVec.x(), normalVec.y(), normalVec.z());

	return normal;
}


bool XShapeSource::update()
{
	//更新数据
	if (!isNeedUpdate()) {
		return false;
	}

	updateFaceColorArray();
	updateIndexArray();
	updateVertextCoordArray();
	updateNormalArray();
	updateVertexColorArray();

	setHasUpdated();
	return true;
}

myUtilty::BoundBox XShapeSource::getBoundBox()
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	myUtilty::Vec3d minBound = myUtilty::Vec3d(limitMax, limitMax, limitMax);
	myUtilty::Vec3d maxBound = myUtilty::Vec3d(limitMin, limitMin, limitMin);

	if (m_coord->size() == 0)
		return myUtilty::BoundBox{ minBound.x(), maxBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};

	float* pData = m_coord->data(0);
	for (int i = 0; i < m_coord->getNumOfTuple(); i++) {

		myUtilty::Vec3d pos = myUtilty::Vec3d(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		minBound = myUtilty::Vec3d(std::min(minBound.x(), pos.x()), std::min(minBound.y(), pos.y()), std::min(minBound.z(), pos.z()));
		maxBound = myUtilty::Vec3d(std::max(maxBound.x(), pos.x()), std::max(maxBound.y(), pos.y()), std::max(maxBound.z(), pos.z()));
	}
	return myUtilty::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};
}

myUtilty::BoundBox XShapeSource::getBoundBox(const Eigen::Affine3f& mat)
{
	constexpr double limitMax = std::numeric_limits<double>::max();
	constexpr double limitMin = std::numeric_limits<double>::lowest();;
	myUtilty::Vec3d minBound = myUtilty::Vec3d(limitMax, limitMax, limitMax);
	myUtilty::Vec3d maxBound = myUtilty::Vec3d(limitMin, limitMin, limitMin);

	if (m_coord->size() == 0)
		return myUtilty::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};

	float* pData = m_coord->data(0);
	for (int i = 0; i < m_coord->getNumOfTuple(); i++) {

		Eigen::Vector3f pos = Eigen::Vector3f(pData[i * 3 + 0], pData[i * 3 + 1], pData[i * 3 + 2]);
		pos = mat * pos;

		minBound = myUtilty::Vec3d(std::min<double>(minBound.x(), pos.x()), std::min<double>(minBound.y(), pos.y()), std::min<double>(minBound.z(), pos.z()));
		maxBound = myUtilty::Vec3d(std::max<double>(maxBound.x(), pos.x()), std::max<double>(maxBound.y(), pos.y()), std::max<double>(maxBound.z(), pos.z()));
	}
	return myUtilty::BoundBox{ minBound.x(), minBound.y(), minBound.z(), maxBound.x(), maxBound.y(), maxBound.z()};
}

void XShapeSource::writeToFile(const std::string& filename)
{
	m_coord->getNumOfTuple();

	std::ofstream file(filename);

	file<< m_coord->getNumOfTuple() << std::endl;

	for (int i = 0; i < m_coord->getNumOfTuple(); i++) {
		file << m_coord->data(i)[0] <<" "<< m_coord->data(i)[1]<<" " <<m_coord->data(i)[2]<<std::endl;
	}

	file << m_indexs->getNumOfTuple() << std::endl;

	for (int i = 0; i < m_indexs->getNumOfTuple(); i++) {
		file << m_indexs->data(i)[0] << " " << m_indexs->data(i)[1] << " " << m_indexs->data(i)[2] << std::endl;
	}
	
	file.close();
}
