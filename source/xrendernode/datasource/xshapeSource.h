#pragma once

#include "../xrendernodeApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataArray.h>

#include <lib00_utilty/XUtilty.h>
#include <dataBase/XVector.h>

class XRENDERNODE_API XShapeSource:public XDataBaseObject {
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

	 std::shared_ptr<XFloatArray> getInstancedArray() { return m_InstanceArray; }

	 std::shared_ptr<XFloatArray> getCustomArray(int name);

	 XQ::Vec3f getFaceNormal(uint32_t index);

	 /**
	  * @brief 更新vbo，用于将CPU端顶点坐标数据更新，同时修改时间戳
	  * @warning isNeedUpdate为true，该接口就会调用，也就是说它的调用时机很容易触发，具体
	  *					   是否需要更新对应的数据(避免频繁更新)，具体的子类需要自己去精细控制，下面的
	  *					   接口类似，当然子类也可以该接口什么事也不做，在外部更新数据也可，只要最会设置
	  *					  source整个的时间戳即可
	  */
	 virtual void updateVertextCoordArray() =0;

	 /**
	 * @brief 更新vbo，用于将CPU端顶点法线数据更新后，将顶点法线信息更新到底层vbo的buffer中
	 */
	 virtual void updateVertextNormalArray() =0;

	 /**
	  * @brief 更新vbo，用于将CPU端顶点颜色数据更新，同时修改时间戳
	  */
	 virtual void updateVertexColorArray()=0;

	 /**
	 * @brief 更新ebo，用于将CPU端三角形索引据更新，同时修改时间戳
	 */
	 virtual void updateFaceIndexArray() = 0;

	 /**
	  * @brief 更新ssbo，用于将CPU端三角形面片颜色数据更新，同时修改时间戳
	  */
	 virtual void updateFaceColorArray() = 0;

	 /**
	  * @brief 更新线fbo，用于更新CPU端线索引数据，同时修改时间戳
	  */
	 virtual void updateLineIndexArray() = 0;

	 /**
	 * @brief 更新线ssbo，用于更新CPU端线片元数据，同时修改时间戳
	 */
	 virtual void updateLineColorArray() = 0;

	 /**
	* @brief 更新点ebo，用于更新CPU端点的索引，同时修改时间戳
	*/
	 virtual void updateVertexIndexArray() = 0;

	 /**
   * @brief 更新实例化矩阵，同时修改时间戳
   */
	 virtual void updateInstancedArray();

	 /**
	  * @brief 更新顶点纹理坐标
      */
	 virtual void updateTextureCoordArray() {};		//目前大部分不需要纹理，因此不使用纯虚函数

	 virtual void updateCustomArray() {};

	virtual bool update();

	virtual XQ::BoundBox getBoundBox();

	virtual XQ::BoundBox getBoundBox(const Eigen::Affine3f& mat);

	void writeToFile(const std::string& filename);

	void Init() override;

	/**
	 * @brief 判断当前数据的更新时间和other的更新时间
	 * @return bool true表示当前对象在other前面更新
	 */
	bool isUpdateBefore(sptr<XShapeSource> other) const {
		return m_updateTime < other->m_updateTime;
	}

	/**
	 * @brief 判断当前数据的更新时间和other的更新时间
	 * @return bool true表示当前对象在other后更新
	 */
	bool isUpdateAfter(sptr<XShapeSource> other) const {
		return m_updateTime > other->m_updateTime;
	}

	virtual void setHasUpdated() { m_updateTime.Modified(); }

protected:
	/**
	 * @brief 判断当前对象是否需要更新
	 * @detail 只要当前对象数据的修改时间更新，即需要更新
	 */
	bool isNeedUpdate() { return m_DataModifyTime > m_updateTime; }
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
	std::shared_ptr<XUIntArray> m_VertexIndexs;										//点索引,它的颜色直接使用顶点颜色属性

	std::shared_ptr<XFloatArray> m_InstanceArray;										//实例化渲染属性,默认始终有一个实例

	std::map<int, std::shared_ptr<XFloatArray>> m_customArray;				//用于自定义属性，目前仅用于文字渲染

	XTimeStamp m_updateTime;
	//仅为了在Init调用update，保证source有数据，
	//有些时候，算法可能需要sourece数据，但是由于未开始绘制，此时renderNode未更新，导致source一直没有数据
	bool initSource = false;	
};