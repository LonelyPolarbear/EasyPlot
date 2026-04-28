#pragma once

#include "../xrendernodeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataArray.h>

#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>

class xrendernode_API XShapeSource:public XDataBaseObject {
protected:
    XShapeSource();
    virtual ~XShapeSource();
public:
	 std::shared_ptr<XFloatArray> getVertextCoordArray(){return m_VertexCoord; }

	 std::shared_ptr<XFloatArray> getVertexNormalArray() { return m_VertexNormal; }

	 std::shared_ptr<XFloatArray> getVertexColorArray() { return m_VertexColor; }

	 std::shared_ptr<XFloatArray> getTextureCoordArray() { return m_TextureCoord; }

	 std::shared_ptr<XUIntArray> getFaceIndexArray() { return m_FaceIndexs; }

	 std::shared_ptr<XFloatArray> getFaceColorArray() { return m_FaceColor; }

	 std::shared_ptr<XUIntArray> getLineIndexArray() { return m_LineIndexs; }

	 std::shared_ptr<XFloatArray> getLineColorArray() { return m_LineColor; }

	 std::shared_ptr<XUIntArray> getVertexIndexArray() { return m_VertexIndexs; }

	 XQ::Vec3f getFaceNormal(uint32_t index);

	 virtual void updateVertextCoordArray() =0;

	 virtual void updateVertextNormalArray() =0;

	 virtual void updateVertexColorArray()=0;

	 virtual void updateFaceIndexArray() = 0;

	 virtual void updateFaceColorArray() = 0;

	 virtual void updateLineIndexArray() = 0;

	 virtual void updateLineColorArray() = 0;

	 virtual void updateVertexIndexArray() = 0;

	 virtual void updateTextureCoordArray(){};		//目前大部分不需要纹理，因此不使用纯虚函数

	virtual bool update();

	virtual XQ::BoundBox getBoundBox();

	virtual XQ::BoundBox getBoundBox(const Eigen::Affine3f& mat);

	void writeToFile(const std::string& filename);

	void Init() override;

	bool isUpdateBefore(sptr<XShapeSource> other) const {
		return m_updateTime < other->m_updateTime;
	}

	bool isUpdateAfter(sptr<XShapeSource> other) const {
		return m_updateTime > other->m_updateTime;
	}

protected:
	bool isNeedUpdate() { return m_DataModifyTime > m_updateTime; }
	void setHasUpdated(){m_updateTime.Modified();}
protected:
	//顶点属性
	std::shared_ptr<XFloatArray> m_VertexCoord;										//顶点坐标
	std::shared_ptr<XFloatArray> m_VertexNormal;										//顶点法线
	std::shared_ptr<XFloatArray> m_VertexColor;										//顶点颜色
	std::shared_ptr<XFloatArray> m_TextureCoord;										//纹理坐标 纹理坐标仅对绘制面片时候生效

	//面单元
	std::shared_ptr<XFloatArray> m_FaceColor;											//面片颜色
	std::shared_ptr<XUIntArray> m_FaceIndexs;											//面片索引

	//线单元
	std::shared_ptr<XFloatArray> m_LineColor;											//线颜色
	std::shared_ptr<XUIntArray> m_LineIndexs;											//线索引

	//点单元
	std::shared_ptr<XUIntArray> m_VertexIndexs;										//点索引

	XTimeStamp m_updateTime;
	//仅为了在Init调用update，保证source有数据，
	//有些时候，算法可能需要sourece数据，但是由于未开始绘制，此时renderNode未更新，导致source一直没有数据
	bool initSource = false;	
};