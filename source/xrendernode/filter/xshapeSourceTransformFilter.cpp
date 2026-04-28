#include <Eigen/Eigen>
#include "xshapeSourceTransformFilter.h"
#include <xalgo/XAlgo.h>

class XShapeSourceTransformFilter::Internal {
public:
	sptr<XShapeSource> input;
	Eigen::Affine3f transform = Eigen::Affine3f::Identity();
	bool isAckInputAtLeastOnce = false;
};

XShapeSourceTransformFilter::XShapeSourceTransformFilter():mData(new Internal)
{
}

XShapeSourceTransformFilter::~XShapeSourceTransformFilter()
{
}

void XShapeSourceTransformFilter::translate(float x, float y, float z)
{
	mData->transform.translate(Eigen::Vector3f(x, y, z));
}

void XShapeSourceTransformFilter::setPosition(float x, float y, float z)
{
	mData->transform.translation() << x, y, z;
}

void XShapeSourceTransformFilter::setScale(float x, float y, float z)
{
	auto data = XQ::Matrix::transformDecomposition_TRS(mData->transform);
	data.sx = x;
	data.sy = y;
	data.sz = z;
	mData->transform.matrix() = XQ::Matrix::computeMatrix(data);
}

void XShapeSourceTransformFilter::rotate(float angle, XQ::Vec3f dir)
{
	Eigen::Vector3f axis = Eigen::Vector3f(dir.x(), dir.y(), dir.z());
	axis.normalize();
	mData->transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), axis));
}

XQ::Vec3f XShapeSourceTransformFilter::getPosition() const
{
	Eigen::Vector3f T= mData->transform.translation();
	return XQ::Vec3f(T[0],T[1],T[2]);
}

XQ::Vec3f XShapeSourceTransformFilter::getScale() const
{
	auto data = XQ::Matrix::transformDecomposition_TRS(mData->transform);
	return XQ::Vec3f(data.sx, data.sy, data.sz);
}

void XShapeSourceTransformFilter::rotateX(float angle)
{
	mData->transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitX()));
}

void XShapeSourceTransformFilter::rotateY(float angle)
{
	mData->transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitY()));
}

void XShapeSourceTransformFilter::rotateZ(float angle)
{
	mData->transform.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(angle), Eigen::Vector3f::UnitZ()));
}

void XShapeSourceTransformFilter::scale(float x, float y, float z)
{
	mData->transform.scale(Eigen::Vector3f(x, y, z));
}

void XShapeSourceTransformFilter::setTransform(const Eigen::Affine3f& trans)
{
	mData->transform = trans;
	Modified();
}

Eigen::Affine3f XShapeSourceTransformFilter::getTransform() const
{
	return mData->transform;
}

void XShapeSourceTransformFilter::setInput(sptr<XShapeSource> input)
{
	mData->input = input;
}

bool XShapeSourceTransformFilter::update()
{
	if(!mData->input)
		return false;
	if (mData->input->update() || isUpdateBefore(mData->input)) {
		//更新自己
		mData->isAckInputAtLeastOnce = true;
		//当前输入有效，更新输出
		return updateSelf();
	}
	else {
		//一种情况是输入从未更新
		//一种是this的变换矩阵更新了
		if (isNeedUpdate()) {
			//矩阵更新
			if(mData->isAckInputAtLeastOnce == false)
				return false;
			//当前输入有效，更新输出
			return updateSelf();
		}
		return false;
	}
}

bool XShapeSourceTransformFilter::updateSelf()
{
	auto inputCoordArray = mData->input->getVertextCoordArray();
	if(inputCoordArray->getNumOfTuple() ==0)
		return false;

	//变换坐标
	{
		auto inputCoordNum = inputCoordArray->getNumOfTuple();
		m_VertexCoord->setNumOfTuple(inputCoordNum);

		XQ::ParaAlgo::ParallelForeach(0, inputCoordNum, [this, inputCoordArray](int i) {
			auto pos = inputCoordArray->data(i);
			Eigen::Vector3f out = mData->transform * Eigen::Vector3f(pos[0], pos[1], pos[2]);
			m_VertexCoord->setTuple(i, out[0], out[1], out[2]);
			});
		m_VertexCoord->Modified();
	}
	
	//变换法线 todo
	{
		m_VertexNormal = mData->input->getVertexNormalArray();
	}

	//其它,不需要变换，直接使用输入的即可
	{
		m_VertexColor = mData->input->getVertexColorArray();

		m_FaceIndexs = mData->input->getFaceIndexArray();
		m_FaceColor = mData->input->getFaceColorArray();

		m_LineIndexs = mData->input->getLineIndexArray();
		m_LineColor = mData	->input->getLineColorArray();

		m_VertexIndexs = mData->input->getVertexIndexArray();

		m_VertexColor->Modified();
		m_FaceIndexs->Modified();
		m_FaceColor->Modified();
		m_LineIndexs->Modified();
		m_LineColor->Modified();
		m_VertexIndexs->Modified();
	}
	setHasUpdated();
	return true;
}
