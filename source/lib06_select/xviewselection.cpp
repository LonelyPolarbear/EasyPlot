#include "xviewselection.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib01_shader/xshader.h"
#include <glew/glew.h>
#include <lib05_shape/xshape.h>
#include <lib02_camera/xcamera.h>


class XViewSelection::Internal {
public:
	std::shared_ptr<xshader> pickShader;

	std::vector< std::shared_ptr<XOpenGLFramebufferObject>> fboPeeling;

	bool isNearToFar = true;
	float depthPeelingEpsilon = 0.0001;
	bool isPeeling = true;
	int PeelLayerNum = 10;
	int currentPass = 1;		//当前的有效层数
	std::vector<unsigned int> queryHandles;
};

XViewSelection::XViewSelection():d(new Internal())
{
}

XViewSelection::~XViewSelection()
{
}

void XViewSelection::setPickShader(std::shared_ptr<xshader> shader)
{
	d->pickShader = shader;
}

bool XViewSelection::create()
{
	bool isComplete = true;
	d->queryHandles.resize(d->PeelLayerNum);
	glGenQueries(d->PeelLayerNum, d->queryHandles.data());
	for (int i = 0; i <d->PeelLayerNum; i++) {
		auto fbo = makeShareDbObject<XOpenGLFramebufferObject>();
		fbo->create();
		fbo->bind();
		fbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA32U, XOpenGLTexture::PixelFormat::RGBA_Integer, XOpenGLTexture::PixelType::UInt32);
		fbo->addAttachment(XOpenGLFramebufferObject::Attachment::Depth, XOpenGLTexture::TextureFormat::D32F, XOpenGLTexture::PixelFormat::Depth, XOpenGLTexture::PixelType::Float32);
		isComplete &= fbo->isComplete();
		fbo->release();
		d->fboPeeling.push_back(fbo);
	}
	return isComplete;
}

bool XViewSelection::updateBufferSize(int width, int height)
{
	//return d->fbo->updateBufferSize(width,height);
	bool result = true;
	for (auto fbo:d->fboPeeling)
	{
		result &= fbo->updateBufferSize(width, height);
	}
	return result;
}

std::shared_ptr<XOpenGLFramebufferObject> XViewSelection::getFbo(int index)
{
	//return d->fbo;
	return d->fboPeeling[index];
}

XViewSelection::SelectData XViewSelection::getPointSelection(int x, int y, int ViewportWidth, int ViewportHeight,  int layer)
{
	SelectData result;
	if (!(x> 0 && x < ViewportWidth && y >0 && y < ViewportHeight)) {
		return result;
	}
	
	//读取当前位置的对象ID
	auto fbo = d->fboPeeling[layer];
	auto pbo = fbo->getColorAttachment()->map();
	if (auto ptrColorTexture = (unsigned int*)pbo->map(XOpenGLBuffer::Access::ReadOnly)) {
		//采样纹理
		auto color1 = ptrColorTexture + (y-1) * ViewportWidth * 4 + (x-1) * 4;
		auto objectId = color1[0];
		auto primitiveId = color1[1];
		result.objectId = objectId;
		result.primitiveId = primitiveId;
	}

	pbo->unmap();
	return result;
}

std::vector<XViewSelection::SelectData> XViewSelection::getAllPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight)
{	
	std::vector<SelectData> results ;
	for (int i = 0; i < d->PeelLayerNum; i++) {
		if (i <= d->currentPass) {
			auto select = getPointSelection(posx, posy, ViewportWidth, ViewportHeight, i);
			results.push_back(select);
		}
	}
	return results;
}

std::vector< std::vector<XViewSelection::SelectData>> XViewSelection::getBoxSelection(int x, int y, int width, int height, int ViewportWidth, int ViewportHeight)
{	
	std::vector< std::vector< SelectData>> result;
	//先缩小范围
	//[0-ViewportWidth]
	//[0-ViewportHeight]
#if 1	
	auto xleft = std::max(x, 0);
	auto xright = std::min(x+width, ViewportWidth);

	auto ybottom = std::max(y, 0);
	auto ytop = std::min(y+height, ViewportHeight);

	if (xleft >= xright || ybottom >= ytop) {
		return result;
	}

	//计算实际范围
	width = xright - xleft;
	height = ytop - ybottom;
	x = xleft;
	y = ybottom;

	for (int i = 0; i < d->PeelLayerNum; i++) {
		if (i > d->currentPass) {
			continue;
		}
		auto fbo = d->fboPeeling[i];
		//读取当前位置的对象ID
		std::vector< SelectData> layerSelect;
		auto pbo = fbo->getColorAttachment()->map();
		if (auto ptrColorTexture = (unsigned int*)pbo->map(XOpenGLBuffer ::Access::ReadOnly)) {
			//采样纹理

			for (int j = 0; j < height; j++) {
				//找到每一行的起始位置
				auto start = ptrColorTexture + (y - 1 + j) * ViewportWidth * 4 + (x - 1) * 4;
				for (int i = 0; i < width; i++) {
					auto color1 = start + i * 4;
					auto objectId = color1[0];
					auto primitiveId = color1[1];
					SelectData s;
					s.objectId = objectId;
					s.primitiveId = primitiveId;
					layerSelect.push_back(s);
				}
			}
		}
		result.push_back(layerSelect);

		pbo->unmap();
	}
#endif
	return result;
}

bool XViewSelection::update(std::set<std::shared_ptr<XShape>> objects, std::shared_ptr<xcamera> camera)
{
	if (d->isPeeling) {
		for (int i = 0; i < d->PeelLayerNum; i++) {
			renderLayer(objects, camera, i);
		}
	}
	else {
		renderLayer(objects, camera, 0);
	}
	return true;
}

bool XViewSelection::renderLayer(std::set<std::shared_ptr<XShape>> objects, std::shared_ptr<xcamera> camera, int layer)
{
	auto fbo = d->fboPeeling[layer];
	fbo->bind();

	if (layer > 0) {
		d->fboPeeling[layer - 1]->getDepthAttachment()->bindUnit(3);
	}

	glViewport(0, 0, fbo->getWidth(), fbo->getHeight());

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glDisable(GL_MULTISAMPLE);

	GLuint clearValue[4] = { 0, 0, 0, 0 };  // R=255, G=0, B=100, A=255

	// 清除颜色缓冲（使用无符号整数清除函数）
	glClearBufferuiv(GL_COLOR, 0, clearValue);

	// 绘制
	d->pickShader->use();
	d->pickShader->setVec2("screenSize",fbo->getWidth(),fbo->getHeight());
	d->pickShader->setInt("CureLayerNum", layer);
	d->pickShader->setBool("isPeeling", d->isPeeling);
	d->pickShader->setBool("isNearToFar", d->isNearToFar);
	d->pickShader->setFloat("depthPeelingEpsilon", 0.0001);

	//查询
	glBeginQuery(GL_SAMPLES_PASSED, d->queryHandles[layer]);

	//d->pickShader->setViewMatrix(camera->getViewMatrix());
	//d->pickShader->setProjectionMatrix(camera->projectionMatrix());

	for (auto shape : objects) {
		shape->setPolygonMode(PolygonMode::fill);
		shape->draw(d->pickShader);
	}

	glEndQuery(GL_SAMPLES_PASSED);

	GLuint fragmentCount = 0;
	glGetQueryObjectuiv(d->queryHandles[layer], GL_QUERY_RESULT, &fragmentCount);
	//auto* ptrColorTexture = (unsigned int*)fbo->getColorAttachment()->map();
	//if (ptrColorTexture) {
		//采样纹理
		//auto color1 = ptrColorTexture[0];
	//}

	//fbo->getColorAttachment()->unmap();


	//auto* ptrDepthTexture = (float*)fbo->getDepthAttachment()->map();
	//if (ptrDepthTexture)
		//auto d1 = ptrDepthTexture[0];
	//fbo->getDepthAttachment()->unmap();

	//std::vector<float>depthValue;
	//depthValue.resize(fbo->getWidth()*fbo->getHeight()*4);
	//fbo->getDepthAttachment()->bind();
	//glGetTexImage(
	//	GL_TEXTURE_2D,
	//	0,
	//	GL_DEPTH_COMPONENT,
	//	GL_FLOAT,
	//	depthValue.data()
	//);
	//fbo->getDepthAttachment()->release();
	if (layer > 0) {
		d->fboPeeling[layer - 1]->getDepthAttachment()->releaseUnit(3);
	}

	fbo->release();
	d->pickShader->unUse();

	return fragmentCount != 0;
}

int XViewSelection::getPeelLayerNum()const {
	return d->PeelLayerNum;
}