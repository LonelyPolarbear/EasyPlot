#include "XViewPicker3D.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib01_shader/xshader.h"
#include <glew/glew.h>
#include <lib05_shape/XGeometryNode.h>
#include <lib02_camera/xcamera.h>
#include <Eigen/Eigen>

#include "XRender.h"
#include "XOpenGLRenderWindow.h"


class XViewPicker3D::Internal {
public:
	std::vector< std::shared_ptr<XOpenGLFramebufferObject>> fboPeeling;
	bool isNearToFar = true;
	float depthPeelingEpsilon = 0.0001;
	bool isPeeling = false;
	int PeelLayerNum = 2;
	int currentPass = 1;														//当前的有效层数

	wptr<XRender> render;
	std::vector<unsigned int> queryHandles;
	bool isInit =false;
};

XViewPicker3D::XViewPicker3D():mData(new Internal())
{
}

XViewPicker3D::~XViewPicker3D()
{
}


bool XViewPicker3D::initGL()
{
	if (mData->isInit) {
		return true;
	}

	auto render =mData->render.lock();
	if(!render)
		return false;

	bool isComplete = true;
	if(!render->makeCurrent())
		return false;

	mData->queryHandles.resize(mData->PeelLayerNum);
	glGenQueries(mData->PeelLayerNum, mData->queryHandles.data());
	for (int i = 0; i < mData->PeelLayerNum; i++) {
		auto fbo = makeShareDbObject<XOpenGLFramebufferObject>();
		fbo->create();
		fbo->bind();
		fbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA32U, XOpenGLTexture::PixelFormat::RGBA_Integer, XOpenGLTexture::PixelType::UInt32);
		fbo->addAttachment(XOpenGLFramebufferObject::Attachment::Depth, XOpenGLTexture::TextureFormat::D32F, XOpenGLTexture::PixelFormat::Depth, XOpenGLTexture::PixelType::Float32);
		isComplete &= fbo->isComplete();
		fbo->release();
		mData->fboPeeling.push_back(fbo);
	}
	mData->isInit = true && isComplete;
	return mData->isInit;
}

std::shared_ptr<XOpenGLFramebufferObject> XViewPicker3D::getFbo(int index)
{
	return mData->fboPeeling[index];
}

XQ::XSelectData XViewPicker3D::getPointSelection(XQ::Vec2i pos)
{
	//return {};
	if (!initGL())
		return {};
	//需要确保初始化资源成功

	if (mData->isPeeling) {
		for (int i = 0; i < mData->PeelLayerNum; i++) {
			renderLayer(i);
		}
	}
	else {
		renderLayer(0);
	}
	if (!makeCurrent())
		return {} ;

	auto render =mData->render.lock();
	
	auto fbo = mData->fboPeeling[0];
	auto pbo = fbo->getColorAttachment()->map();

	XQ::XSelectData result;

	auto windowViewport = render->getConvertViewPort();
	auto ViewportWidth = fbo->getWidth();
	
	//auto point = render->window2render(pos);
	int x = pos[0];
	int y = pos[1];
	auto ddd = pbo->map2cpu();
	if (auto ptrColorTexture = (unsigned int*)pbo->map(XOpenGLBuffer::Access::ReadOnly)) {
		//采样纹理
		auto color1 = ptrColorTexture + (y - 1) * ViewportWidth * 4 + (x - 1) * 4;
		auto objectId = color1[0];
		auto primitiveId = color1[1];
		result.objectId = objectId;
		result.primitiveId = primitiveId;
	}

	pbo->unmap();
	doneCurrent();

	return result;
}


bool XViewPicker3D::renderLayer(int layer)
{
	//着色器参数设置
	auto render = mData->render.lock();
	if(!render)
		return false;
	auto manger =render->getRenderWindow()->getShaderManger();
	if(!manger)
		return false;
	auto pick_shader = manger->getPickShader3D();

	if(!makeCurrent())
		return false;

	//设置framebuffer
	auto fbo = mData->fboPeeling[layer];
	fbo->bind();

	if (layer > 0) {
		mData->fboPeeling[layer - 1]->getDepthAttachment()->bindUnit(3);
	}

	pick_shader->use();
	pick_shader->setVec2("screenSize", fbo->getWidth(), fbo->getHeight());
	pick_shader->setInt("CureLayerNum", layer);
	pick_shader->setBool("isPeeling", mData->isPeeling);
	pick_shader->setBool("isNearToFar", mData->isNearToFar);
	pick_shader->setFloat("depthPeelingEpsilon", 0.0001);

	if (auto r = mData->render.lock()) {
		r->render(false);
	}
	fbo->release();
	return true;
}

int XViewPicker3D::getPeelLayerNum()const {
	return mData->PeelLayerNum;
}

void XViewPicker3D::setRender(sptr<XRender> render)
{
	mData->render = render;
}

bool XViewPicker3D::slotRenderSizeChanged(XQ::Vec2i size)
{
	if(!initGL())
		return false;
	makeCurrent();
	bool result = true;
	auto render =mData->render.lock();
	auto v = render->getConvertViewPort();
	for (auto fbo : mData->fboPeeling)
	{
		result &= fbo->updateBufferSize(size[0],size[1]);
	}
	doneCurrent();
	return result;
}

bool XViewPicker3D::makeCurrent()
{
	if (auto render = mData->render.lock()) {
		return render->makeCurrent();
	}
	return false;
}

void XViewPicker3D::doneCurrent()
{
	if (auto render = mData->render.lock()) {
		return render->doneCurrent();
	}
}
