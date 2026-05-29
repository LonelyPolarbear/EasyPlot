#include "XTextMapper.h"
#include "xrendernode/datasource/xshapeSource.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>

#define ATTR_VERTEX_INSTNCE_TEXTURE_COORD 8
class XTextMapper::Internal {
	public:
		sptr<XOpenGLBuffer> texCoordBuffer;
		
		Internal() {
			texCoordBuffer = makeShareDbObject<XOpenGLBuffer>();
		}
};
XTextMapper::XTextMapper():mData(new Internal)
{

}

XTextMapper::~XTextMapper()
{

}

void XTextMapper::InitRenderResource()
{
	XPolyDataMapper::InitRenderResource();

	mData->texCoordBuffer->setBufferType(XOpenGLBuffer::VertexBuffer);

	mData->texCoordBuffer->setUsagePattern(XOpenGLBuffer::StaticDraw);

	mData->texCoordBuffer->create();
}

void XTextMapper::updateData()
{
	XPolyDataMapper::updateData();

	//更新子类自己的数据
	auto InstanceTextCoordArray = m_Input->getCustomArray(0);		//实例化纹理坐标
	if (InstanceTextCoordArray && InstanceTextCoordArray->GetTimeStamp() > m_UpdateTime) {
		
			mData->texCoordBuffer->bind();

			mData->texCoordBuffer->allocate(InstanceTextCoordArray->data(0), InstanceTextCoordArray->size());

			mData->texCoordBuffer->release();

			if (InstanceTextCoordArray->getNumOfTuple()) {
				m_vao->addBuffer(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 0, mData->texCoordBuffer, 3, XOpenGL::DataType::float_, sizeof(float) *12, 0, true);
				m_vao->addBuffer(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 1, mData->texCoordBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 3, true);
				m_vao->addBuffer(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 2, mData->texCoordBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 6, true);
				m_vao->addBuffer(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 3, mData->texCoordBuffer, 3, XOpenGL::DataType::float_, sizeof(float) * 12, sizeof(float) * 9, true);

				m_vao->enableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 0);
				m_vao->enableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 1);
				m_vao->enableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 2);
				m_vao->enableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 3);
			}
			else {
				m_vao->disableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 0);
				m_vao->disableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 1);
				m_vao->disableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 2);
				m_vao->disableAttribute(ATTR_VERTEX_INSTNCE_TEXTURE_COORD + 3);
			}
	}
}

