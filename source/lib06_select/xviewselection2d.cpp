#include "xviewselection2d.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib01_shader/xshader.h"
#include <glew/glew.h>
#include <lib05_shape/XGraphicsItem.h>
#include <lib05_shape/XScreenTextItem.h>
#include <lib02_camera/xcamera.h>

class XViewSelection2D::Internal {
public:
	std::shared_ptr<xshader> pickShader;
	std::shared_ptr<xshader> pickFillShader;	//用于拾取填充区域

	std::vector< std::shared_ptr<XOpenGLFramebufferObject>> fboPeeling;

	bool isNearToFar = true;
	float depthPeelingEpsilon = 0.0001;
	bool isPeeling = true;
	int PeelLayerNum = 10;
	int currentPass = 1;		//当前的有效层数
	std::vector<unsigned int> queryHandles;

	std::weak_ptr<DataBaseObject> scene;	//拾取记录场景的weakptr，对于一些屏幕显示对象，需要获取场景到屏幕的变换矩阵，该信息在拾取时需要使用
	std::function<Eigen::Matrix4f(std::shared_ptr<DataBaseObject>)> screenPos2ScenePosFn;
};

XViewSelection2D::XViewSelection2D():d(new Internal())
{
}

XViewSelection2D::~XViewSelection2D()
{
}

void XViewSelection2D::setPickShader(std::shared_ptr<xshader> shader)
{
	d->pickShader = shader;
}

void XViewSelection2D::setPickFillShader(std::shared_ptr<xshader> shader)
{
	d->pickFillShader = shader;
}

void XViewSelection2D::setScene(std::shared_ptr<DataBaseObject> scene)
{
	d->scene = scene;
}

void XViewSelection2D::setGetMatrixforScreen2Scene(std::function<Eigen::Matrix4f(std::shared_ptr<DataBaseObject>)> fn)
{
	d->screenPos2ScenePosFn = fn;
}

bool XViewSelection2D::create()
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

bool XViewSelection2D::updateBufferSize(int width, int height)
{
	bool result = false;
	for (auto fbo:d->fboPeeling)
	{
		result &= fbo->updateBufferSize(width, height);
	}
	return result;
}

std::shared_ptr<XOpenGLFramebufferObject> XViewSelection2D::getFbo(int index)
{
	return d->fboPeeling[index];
}

XViewSelection2D::SelectData XViewSelection2D::getPointSelection(int x, int y, int ViewportWidth, int ViewportHeight,  int layer)
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

std::vector<XViewSelection2D::SelectData> XViewSelection2D::getAllPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight)
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

std::vector< std::vector<XViewSelection2D::SelectData>> XViewSelection2D::getBoxSelection(int x, int y, int width, int height, int ViewportWidth, int ViewportHeight)
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
		if (auto ptrColorTexture = (unsigned int*)pbo->map(XOpenGLBuffer::Access::ReadOnly)) {
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

int XViewSelection2D::update(std::set<std::shared_ptr<XGraphicsItem>> objects, std::shared_ptr<xcamera> camera,const Eigen::Matrix4f& m)
{
	if (d->isPeeling) {
		for (int i = 0; i < d->PeelLayerNum; i++) {
			if (!renderLayer(objects, camera, i, m)) {
				d->currentPass = i-1;
				return i;
			}	
		}
		d->currentPass = d->PeelLayerNum;
		return  d->PeelLayerNum;
	}
	else {
		renderLayer(objects, camera, 0, m);
		d->currentPass = 0;
		
	}
	return 0;
}

bool XViewSelection2D::renderLayer(std::set<std::shared_ptr<XGraphicsItem>> objects, std::shared_ptr<xcamera> camera, int layer, const Eigen::Matrix4f& m)
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
	//查询
	glBeginQuery(GL_SAMPLES_PASSED, d->queryHandles[layer]);

	{
		//线条绘制拾取
		d->pickShader->use();
		d->pickShader->setVec2("screenSize", fbo->getWidth(), fbo->getHeight());
		d->pickShader->setInt("CureLayerNum", layer);
		d->pickShader->setBool("isPeeling", d->isPeeling);
		d->pickShader->setBool("isNearToFar", d->isNearToFar);
		d->pickShader->setFloat("depthPeelingEpsilon", 0.0001);


		for (auto shape : objects) {
			shape->setPolygonMode(PolygonMode::line);
			shape->pickBorderDraw(d->pickShader, m);
		}

		d->pickShader->unUse();
	}

	{	
		//填充拾取
		d->pickFillShader->use();
		d->pickFillShader->setVec2("screenSize", fbo->getWidth(), fbo->getHeight());
		d->pickFillShader->setInt("CureLayerNum", layer);
		d->pickFillShader->setBool("isPeeling", d->isPeeling);
		d->pickFillShader->setBool("isNearToFar", d->isNearToFar);
		d->pickFillShader->setFloat("depthPeelingEpsilon", 0.0001);

		d->pickFillShader->setViewMatrix(camera->getViewMatrix());
		d->pickFillShader->setProjectionMatrix(camera->projectionMatrix());
		for (auto shape : objects) {
			//需要确保填充模式下生效
			auto old = shape->isFilled();
			shape->setIsFilled(true);
			shape->pickFillDraw(d->pickFillShader, m);
			shape->setIsFilled(old);
		}
		d->pickFillShader->unUse();
	}

	GLuint fragmentCount = 0;
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectuiv(d->queryHandles[layer], GL_QUERY_RESULT, &fragmentCount);

	
	if (layer > 0) {
		d->fboPeeling[layer - 1]->getDepthAttachment()->releaseUnit(3);
	}

	fbo->release();
	return fragmentCount != 0;
}

int XViewSelection2D::getPeelLayerNum()const {
	return d->PeelLayerNum;
}