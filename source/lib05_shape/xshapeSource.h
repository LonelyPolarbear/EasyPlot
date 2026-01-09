#pragma once

#include "xshapeApi.h"
#include <dataBase/dataobject.h>
#include <dataBase/XDataArray.h>

#include <lib00_utilty/myUtilty.h>

class LIB05_SHAPE_API XShapeSource:public DataBaseObject {
protected:
    XShapeSource();
    virtual ~XShapeSource();
public:
	 std::shared_ptr<XFloatArray> getVertextCoordArray(){return m_coord; }

	 std::shared_ptr<XUIntArray> getIndexArray() { return m_indexs; }

	 std::shared_ptr<XFloatArray> getFaceColorArray() { return m_FaceColor; }

	 std::shared_ptr<XFloatArray> getNormalArray() { return m_normal; }

	 std::shared_ptr<XFloatArray> getVertexColorArray() { return m_VertexColor; }

	 XQ::Vec3f getFaceNormal(uint32_t index);

	 virtual void updateVertextCoordArray() =0;

	 virtual void updateIndexArray() =0;

	 virtual void updateFaceColorArray() =0;

	 virtual void updateNormalArray() =0;

	 virtual void updateVertexColorArray()=0;

	virtual bool update();

	virtual XQ::BoundBox getBoundBox();

	virtual XQ::BoundBox getBoundBox(const Eigen::Affine3f& mat);

	void writeToFile(const std::string& filename);

	void Init() override;

protected:
	bool isNeedUpdate() { return m_DataModifyTime > m_updateTime; }
	void setHasUpdated(){m_updateTime.Modified();}
protected:
	std::shared_ptr<XFloatArray> m_coord;
	std::shared_ptr<XFloatArray> m_normal;
	std::shared_ptr<XFloatArray> m_VertexColor;
	std::shared_ptr<XFloatArray> m_FaceColor;
	std::shared_ptr<XUIntArray> m_indexs;

	XTimeStamp m_updateTime;
};