#pragma once
#include <xrendernode/xrendernodeApi.h>
#include <dataBase/XDataBaseObject.h>
#include <xrendernode/XRenderNode.h>
#include <lib01_shader/XBaseShader.h>
#include <Eigen/Eigen>

class XOpenGLVertexArrayObject;
class XOpenGLBuffer;
class XShapeSource;
class xshader;
class XRENDERNODE_API XPolyDataMapper :public XDataBaseObject {
protected:
	XPolyDataMapper();
	~XPolyDataMapper();
public:
	void setInput(std::shared_ptr<XShapeSource> input);

	std::shared_ptr<XShapeSource> getInput() const {
		return m_Input;
	}

	void Init() override;

	virtual void updateData();

	virtual void draw(sptr<xshader> shader, PolygonMode mode, PrimitveType type);

	void initiallize();

	void initGLResource();

	void bindSSBO();

protected:
	//GPU端对象
	sptr<XOpenGLVertexArrayObject> m_vao;						//VAO
	sptr<XOpenGLBuffer> m_vertex_coord;							//顶点坐标，必须有
	sptr<XOpenGLBuffer> m_vertex_normal;							//点的法线
	sptr<XOpenGLBuffer> m_vertex_color;							//点的颜色
	sptr<XOpenGLBuffer> m_vertex_textureCoord;				//点的纹理坐标

	sptr<XOpenGLBuffer> m_face_color;								//每个片元的颜色
	sptr<XOpenGLBuffer> m_face_ebo;									//面片绘制的索引

	sptr<XOpenGLBuffer> m_line_color;								//每个线的颜色  目前不允许设置线的颜色，直接使用singleColor
	sptr<XOpenGLBuffer> m_line_ebo;									//线的索引

	sptr<XOpenGLBuffer> m_point_ebo;								//点的索引，默认显示全部点
	
	//CPU端数据
	sptr<XShapeSource> m_Input;

	bool isGLResourceInit = false;

	XTimeStamp m_UpdateTime;
};