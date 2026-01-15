#pragma once
#include <lib05_shape/xshapeApi.h>
#include <dataBase/XDataBaseObject.h>
#include <lib05_shape/XRenderNode.h>
#include <lib01_shader/XBaseShader.h>

class XOpenGLVertexArrayObject;
class XOpenGLBuffer;
class XShapeSource;
class xshader;
class LIB05_SHAPE_API XPolyDataMapper :public XDataBaseObject {
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
	sptr<XOpenGLVertexArrayObject> m_vao;					//VAO
	sptr<XOpenGLBuffer> m_vbo_coord;							//顶点坐标，必须有
	sptr<XOpenGLBuffer> m_vbo_normal;							//法线
	sptr<XOpenGLBuffer> m_vbo_color;								//颜色
	sptr<XOpenGLBuffer> m_ssbo_color;							//每个片元的颜色
	sptr<XOpenGLBuffer> m_ebo;										//EBO

	//CPU端数据
	sptr<XShapeSource> m_Input;
	bool isGLResourceInit = false;

	XTimeStamp m_UpdateTime;
};