#include <Eigen/Eigen>
#include "xshapeSourceCombineFilter.h"
#include <xalgo/XAlgo.h>

class XShapeSourceCombineFilter::Internal {
public:
	std::vector< sptr<XShapeSource>> inputs;
	
	bool isAckInputAtLeastOnce = false;
};

XShapeSourceCombineFilter::XShapeSourceCombineFilter():mData(new Internal)
{
}

XShapeSourceCombineFilter::~XShapeSourceCombineFilter()
{
}

void XShapeSourceCombineFilter::addInput(sptr<XShapeSource> input)
{	
	if(input)
		mData->inputs.push_back(input);
}

bool XShapeSourceCombineFilter::update()
{
	bool updateFlag = false;
	for (auto i : mData->inputs) {
		auto r= i->update();
		updateFlag |= r;
	}
	if (isNeedUpdate() || updateFlag) {
		return updateSelf();
	}
	return false;
}

bool XShapeSourceCombineFilter::updateSelf()
{
	//组装数据,总体原则，顶点坐标必须组装
	//顶点坐标，法向量，颜色
	//面索引 面颜色
	//线索引 线颜色
	//点索引
	auto totalNumOfVertex = 0;
	auto totalNumOfVertexNormal = 0;
	auto totalNumOfVertexColor = 0;
	auto totalNumOfFaceColor = 0;
	std::vector<uint32_t> vertexOffset;
	{
		vertexOffset.push_back(0);
		for (auto s : mData->inputs) {
			totalNumOfVertex += s->getVertextCoordArray()->getNumOfTuple();
			totalNumOfVertexNormal += s->getVertexNormalArray()->getNumOfTuple();
			totalNumOfVertexColor += s->getVertexColorArray()->getNumOfTuple();
			totalNumOfFaceColor += s->getFaceColorArray()->getNumOfTuple();
			vertexOffset.push_back(totalNumOfVertex);
		}
	}

	//顶点坐标
	{
		//合并坐标
		std::vector< sptr< XFloatArray>> coords;
		for (auto d : mData->inputs) {
			coords.push_back(d->getVertexColorArray());
		}
		XQ::XAlgo::combineArray(coords, m_VertexCoord);
	}
		
	//顶点法向量和顶点颜色
	{
		//顶点法向量和顶点颜色原则上合并必须保证都有，要么直接丢弃
		if (totalNumOfVertexNormal == totalNumOfVertex) {
			std::vector< sptr< XFloatArray>> normals;
			for (auto d : mData->inputs) {
				normals.push_back(d->getVertexNormalArray());
			}
			XQ::XAlgo::combineArray(normals, m_VertexNormal);
		}

		if (totalNumOfVertexColor == totalNumOfVertex) {
			std::vector< sptr< XFloatArray>> vertex_color;
			for (auto d : mData->inputs) {
				vertex_color.push_back(d->getVertextCoordArray());
			}
			XQ::XAlgo::combineArray(vertex_color, m_VertexColor);
		}
	}

	//面索引和面颜色
	{
		std::vector< sptr< XUIntArray>> face_index;
		for (auto d : mData->inputs) {
			face_index.push_back(d->getFaceIndexArray());
		}
		XQ::XAlgo::combineArray(face_index, m_FaceIndexs);

		//面索引
		auto totalNumOfFace = 0;
		for (int i = 0; i < mData->inputs.size(); i++) {
			//创建一个新的，同时将数据偏移 vertexOffset[i] 
			auto faceNum = mData->inputs[i]->getFaceIndexArray()->getNumOfTuple();		//面数量
			auto startFace = totalNumOfFace;
			auto offset = vertexOffset[i];
			m_FaceIndexs->foreach_tuple_range(startFace,faceNum,[&,i](unsigned int *p){
				p[0] += offset;
				p[1] += offset;
				p[2] += offset;
			});

			totalNumOfFace += faceNum;
		}

		//面颜色
		if (totalNumOfFaceColor == totalNumOfFace) {
			std::vector< sptr< XFloatArray>> face_color;
			for (auto d : mData->inputs) {
				face_color.push_back(d->getFaceColorArray());
			}
			XQ::XAlgo::combineArray(face_color, m_FaceColor);
		}
	}

	setHasUpdated();
	return true;
}
