#include "xscene.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLContext.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "lib01_shader/xshader.h"
#include "lib01_shader/xshaderManger.h"
#include "lib06_select/xviewselection.h"
#include "lib06_select/xviewselection2d.h"
#include <glew/glew.h>
#include <lib05_shape/xshape.h>
#include <lib05_shape/XGraphicsItem.h>
#include <lib05_shape/XRectItem.h>
#include <lib05_shape/XGridItem.h>
#include <lib05_shape/XScreenTextItem.h>
#include <lib02_camera/xcamera.h>
#include <set>
#include <iostream>
#include <lib05_shape/xCustomSource.h>
#include <lib05_shape/xchamferCubeSource.h>

#include <dataBase/XDataArray.h>

#include <Eigen/Eigen>
#include <lib03_stbImage/stbImage.h>

#include <future>
#include <limits.h>
bool isScreenRender = true;
class SceneUbo:public DataBaseObject{
protected:
    SceneUbo(){}
    ~SceneUbo(){}

public:
	void Create() {
		uniformBufferVs = makeShareDbObject<XOpenGLBuffer>();
		uniformBufferVs->setBufferType(XOpenGLBuffer::UniformBuffer);
		uniformBufferVs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
		uniformBufferVs->create();
		uniformBufferVs->bind();
		uniformBufferVs->allocate(sizeof(Eigen::Matrix4f) * 2);
		uniformBufferVs->setBufferBindIdx(1);
		uniformBufferVs->release();

		uniformBufferVs2d = makeShareDbObject<XOpenGLBuffer>();
        uniformBufferVs2d->setBufferType(XOpenGLBuffer::UniformBuffer);
        uniformBufferVs2d->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
        uniformBufferVs2d->create();
        uniformBufferVs2d->bind();
        uniformBufferVs2d->allocate(sizeof(Eigen::Matrix4f) * 2);
        uniformBufferVs2d->setBufferBindIdx(4);
        uniformBufferVs2d->release();


        uniformBufferFs = makeShareDbObject<XOpenGLBuffer>();
        uniformBufferFs->setBufferType(XOpenGLBuffer::UniformBuffer);
        uniformBufferFs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
        uniformBufferFs->create();
        uniformBufferFs->bind();
        uniformBufferFs->allocate(sizeof(Eigen::Vector2f) * 4);
        uniformBufferFs->setBufferBindIdx(2);
        uniformBufferFs->release();

        uniformBufferGs = makeShareDbObject<XOpenGLBuffer>();
        uniformBufferGs->setBufferType(XOpenGLBuffer::UniformBuffer);
        uniformBufferGs->setUsagePattern(XOpenGLBuffer::UsagePattern::StaticDraw);
        uniformBufferGs->create();
        uniformBufferGs->bind();
        uniformBufferGs->allocate(sizeof(Eigen::Vector2f) * 2+4);
        uniformBufferGs->setBufferBindIdx(3);
        uniformBufferGs->release();
	}

    void writeVS(const Eigen::Matrix4f& view, const Eigen::Matrix4f& projection) {
		uniformBufferVs->bind();
		uniformBufferVs->write(0, view.data(), 16);
		uniformBufferVs->write(16, projection.data(), 16);
		uniformBufferVs->release();
    }

	void writeVS2d(const Eigen::Matrix4f& virtualWorldFrame, const Eigen::Matrix4f& sceneFrameInVirtualWorld) {
		uniformBufferVs2d->bind();
        uniformBufferVs2d->write(0, virtualWorldFrame.data(), 16);
        uniformBufferVs2d->write(16, sceneFrameInVirtualWorld.data(), 16);
        uniformBufferVs2d->release();
	}
    
    void writeFS(const Eigen::Vector2f& viewport, const Eigen::Vector2f& mousePos) {
		uniformBufferFs->bind();
        uniformBufferFs->write(0, viewport.data(), 2);
		uniformBufferFs->write(2, mousePos.data(), 2);
        uniformBufferFs->release();
    }

	void writeGS(const Eigen::Vector2f& viewport,float sceneScale) {
        uniformBufferGs->bind();
        uniformBufferGs->write(0, viewport.data(), 2);
        uniformBufferGs->write(2, &sceneScale, 1);
        uniformBufferGs->release();
	}
protected:
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs;               //用于传递变换矩阵的缓冲区
	std::shared_ptr<XOpenGLBuffer> uniformBufferVs2d;           //用于传递变换矩阵的缓冲区
	std::shared_ptr<XOpenGLBuffer> uniformBufferFs;               //
	std::shared_ptr<XOpenGLBuffer> uniformBufferGs;               //
};

class XScene::Internal {
public:
	std::set<std::shared_ptr<XShape>> shapes;                               //3D物体集合
	std::set<std::shared_ptr<XGraphicsItem>> shapes2D;              //2D物体集合
	std::shared_ptr<xShaderManger> shaderManger;
	std::shared_ptr<xcamera> camera;
	std::shared_ptr<XViewSelection> viewSelection;
	std::shared_ptr<XViewSelection2D> viewSelection2D;
    std::shared_ptr<XOpenGLContext> context;

    std::shared_ptr < XOpenGLTexture> fontTexture;

    std::shared_ptr<SceneUbo> sceneUbo;

	std::shared_ptr<XShape> gridShape;
	std::shared_ptr<XGraphicsItem> gridShape2d;
	std::shared_ptr<XShape> axisShape;

    bool fontinitialized{ false };
    std::future<std::tuple<int, int, std::vector<const void*>>> result_future;

	void createGrid() {
		if (!gridShape) {
			gridShape = makeShareDbObject<XShape>();
            gridShape->setVisible(false);
			gridShape->initResource();
            auto gridSource = makeShareDbObject<XCustomSource>();
            auto coord = gridSource->getVertextCoordArray();
            coord->setNumOfTuple(4);
            coord->setTuple(0, -1,-1,-1);
            coord->setTuple(1, 1,-1,-1);
            coord->setTuple(2, 1,1,-1);
            coord->setTuple(3, -1,1,-1);
            coord->Modified();

            auto index = gridSource->getIndexArray();
            
            index->setNumOfTuple(6);
            index->setTuple(0, 0,1,2);
            index->setTuple(1, 0,2,3);
            index->Modified();

            gridSource->Modified();

            gridShape->setColorMode(ColorMode::SingleColor);
            gridShape->setSingleColor(XQ::Vec4f(1,1,1,1));

            gridShape->setInput(gridSource);
		}
	}

	void createGrid2d() {
		if (!gridShape2d) {            
            auto grid = makeShareDbObject<XGridItem>();
            //grid->initiallize();
            gridShape2d = grid;
            gridShape2d->setShaderManger(shaderManger);
            gridShape2d->setVisible(false);
		}
	}

	void createAxisShape() {
		if (!axisShape) {
            axisShape = makeShareDbObject<XShape>();
            axisShape->setVisible(false);
            axisShape->initResource();
			auto axisSource = makeShareDbObject<xchamferCubeSource>();
            axisShape->setColorMode(ColorMode::FaceColor);
            axisShape->setInput(axisSource);
            axisShape->setShaderManger(shaderManger);
		}
	}
    
    void createViewSelection3D() {
		if (!viewSelection) {
			viewSelection = makeShareDbObject<XViewSelection>();
			viewSelection->create();
            viewSelection->setPickShader(shaderManger->getPickShader3D());
			viewSelection->updateBufferSize(getViewportWidth(), getViewportHeight());
		}
    }

	void createViewSelection2D(std::shared_ptr<DataBaseObject> scene,std::function<Eigen::Matrix4f(std::shared_ptr<DataBaseObject>)> f ) {
		if (!viewSelection2D) {
            viewSelection2D = makeShareDbObject<XViewSelection2D>();
            viewSelection2D->create();
			viewSelection2D->setPickFillShader(shaderManger->getPickFillShader2D());
			viewSelection2D->setPickShader(shaderManger->getPickShader2D());
            viewSelection2D->updateBufferSize(getViewportWidth(), getViewportHeight());
            viewSelection2D->setGetMatrixforScreen2Scene(f);
            viewSelection2D->setScene(scene);
		}
	}

    //为了方便把世界坐标系下的点与屏幕坐标系建立联系，使用一个虚拟的世界坐标系，它由当前的相机坐标系变换得到
	Eigen::Affine3f virtualWorldFrame = Eigen::Affine3f::Identity();

	Eigen::Affine3f sceneFrameInVirtualWorld = Eigen::Affine3f::Identity();     //场景坐标系，是相对于虚拟世界坐标系的坐标系

    //视口坐标系 坐标原点位于左下角 X轴向右 Y轴向上 标识当前场景的视口在屏幕坐标系下的位置,屏幕坐标系原点在左下角
	Eigen::Affine3f viewportFrame = Eigen::Affine3f::Identity();

    int width = 0;                                                              //场景的宽度
    int height = 0;                                                             //场景的高度
    int startx = 0;                                                             //场景的起始x坐标
    int starty = 0;                                                             //场景的起始y坐标
   
    bool addShape(std::shared_ptr<XShape> shape)
    {
        return shapes.insert(shape).second;
    }

	bool addGraphicsItem(std::shared_ptr<XGraphicsItem> shape)
	{
		return shapes2D.insert(shape).second;
	}

    bool removeShape(std::shared_ptr<XShape> shape)
    {
        return shapes.erase(shape) > 0;
    }

	bool removeShape2d(std::shared_ptr<XGraphicsItem> shape)
	{
		return shapes2D.erase(shape) > 0;
	}

    std::shared_ptr<XShape> getShape(uint64_t id) {
        for (auto& shape : shapes) {
            if (shape->getID() == id) {
                return shape;
            }
        }
        return nullptr;
    }

	std::shared_ptr<XGraphicsItem> getShape2d(uint64_t id) {
		for (auto& shape : shapes2D) {
			if (auto ret =shape->getChildByID(id)) {
				return ret;
			}
		}
		return nullptr;
	}

    void setcamera(std::shared_ptr<xcamera> camera)
    {
        this->camera = camera;
    }

    void setviewSelection(std::shared_ptr<XViewSelection> viewSelection)
    {
        this->viewSelection = viewSelection;
    }

    void setShaderManger(std::shared_ptr<xShaderManger> shader)
    {
        this->shaderManger = shader;
    }

    void setViewport(int x, int y, int w, int h)
    {
        this->width = w;
        this->height = h;
        this->startx =x;
        this->starty = y;
    }

    std::shared_ptr<xcamera> getcamera()
    {
        return camera;
    }

    std::shared_ptr<XViewSelection> getviewSelection()
    {
        return viewSelection;
    }

    std::shared_ptr<xShaderManger> getShaderManger()
    {
        return shaderManger;
    }

    int getViewportWidth()
    {
        return width;
    }

    int getViewportHeight()
    {
        return height;
    }

};

XScene::XScene():d(new Internal())
{
}

XScene::~XScene()
{
}

bool XScene::addShape(std::shared_ptr<XShape> shape)
{
    if (shape) {
        shape->setShaderManger(d->shaderManger);
        return d->addShape(shape);
    }
    return false;
}

bool XScene::addGraphicsItem(std::shared_ptr<XGraphicsItem> shape)
{
    if(shape){
        shape->setShaderManger(d->shaderManger);
    }
    return d->addGraphicsItem(shape);
}

bool XScene::removeShape(std::shared_ptr<XShape> shape)
{
    return d->removeShape(shape);
}

bool XScene::removeGraphicsItem(std::shared_ptr<XGraphicsItem> shape)
{
    return d->removeShape2d(shape);
}

void XScene::removeAll() {
    while (d->shapes.size() != 0) {
        d->removeShape(*d->shapes.begin());
    }

	while (d->shapes2D.size() != 0) {
		d->removeShape2d(*d->shapes2D.begin());
	}
}

std::shared_ptr<XShape> XScene::getShape(uint64_t id)
{
    return d->getShape(id);
}

std::shared_ptr<XGraphicsItem> XScene::getGraphicsItem(uint64_t id)
{
    return d->getShape2d(id);
}


void XScene::setCamera(std::shared_ptr<xcamera> camera)
{
    d->setcamera(camera);
}

std::shared_ptr<xcamera> XScene::getCamera()
{
    return d->getcamera();
}

void XScene::setContext(std::shared_ptr<XOpenGLContext> context)
{
    d->context = context;
}

std::shared_ptr<XOpenGLContext> XScene::getContext()
{
    return d->context;
}

void XScene::setShaderManger(std::shared_ptr<xShaderManger> shader)
{
    d->setShaderManger(shader);
}

std::shared_ptr<xShaderManger> XScene::getShaderManger()
{
    return d->getShaderManger();
}

void XScene::setViewSelection(std::shared_ptr<XViewSelection> viewselection)
{
    d->setviewSelection(viewselection);
}

std::shared_ptr<XViewSelection> XScene::getViewSelection()
{
    return d->getviewSelection();
}

void XScene::setViewport(int x, int y, int w, int h)
{
    d->setViewport(x,y,w, h);
}

int XScene::getViewportWidth() const
{
    return d->getViewportWidth();
}

int XScene::getViewportHeight() const
{
    return d->getViewportHeight();
}

void XScene::updateViewport()
{
    glViewport(d->startx, d->starty, d->width, d->height);
    updateVirtualWorldFrame();
}

/// <summary>
/// 
/// </summary>
/// <param name="x">屏幕坐标系,原点在左下角</param>
/// <param name="y">屏幕坐标系,原点在左下角</param>
/// <returns></returns>
std::vector<XViewSelection::SelectData> XScene::getPointSelection(int x, int y)
{
	makeCurrent();
    d->createViewSelection3D();

	d->viewSelection->update(d->shapes, d->camera);
	auto selectData = d->viewSelection->getAllPointSelection(x - d->startx, y - d->starty, d->width, d->height);
	doneCurrent();
    return selectData;
}

std::vector<XViewSelection2D::SelectData> XScene::getPointSelection2D(int x, int y)
{
	makeCurrent();
    d->createViewSelection2D(this->shared_from_this(),&XScene::sceneScreenPos2ScenePosMat);
	d->viewSelection2D->update(d->shapes2D, d->camera, Eigen::Matrix4f::Identity());
	auto selectData = d->viewSelection2D->getAllPointSelection(x - d->startx, y - d->starty, d->width, d->height);
	doneCurrent();
	return selectData;
}

std::vector<std::vector<XViewSelection::SelectData>> XScene::getBoxSelection(int x, int y, int w, int h)
{
	makeCurrent();
	d->createViewSelection3D();
    d->viewSelection->update(d->shapes, d->camera);
   auto r =  d->viewSelection->getBoxSelection(x,y,w,h,getViewportWidth(),getViewportHeight());
   doneCurrent();
   return r;
}

std::vector<std::vector<XViewSelection2D::SelectData>> XScene::getBoxSelection2D(int x, int y, int w, int h)
{
	makeCurrent();
	d->createViewSelection2D(this->shared_from_this(), &XScene::sceneScreenPos2ScenePosMat);
	d->viewSelection2D->update(d->shapes2D, d->camera,  Eigen::Matrix4f::Identity());
	auto r = d->viewSelection2D->getBoxSelection(x, y, w, h, getViewportWidth(), getViewportHeight());
	doneCurrent();
	return r;
}

void XScene::updateUbo2GPU()
{
    createUbo();
    d->sceneUbo->writeVS(d->camera->getViewMatrix(), d->camera->projectionMatrix());
	d->sceneUbo->writeFS(Eigen::Vector2f(d->width, d->height), Eigen::Vector2f(mousePosx - d->startx, mousePosy - d->starty));
    d->sceneUbo->writeVS2d(d->virtualWorldFrame.matrix(),d->sceneFrameInVirtualWorld.matrix());
    //提取缩放因子
	Eigen::Matrix3f rotMatrix;
	Eigen::Matrix3f scaleMatrix;
	d->sceneFrameInVirtualWorld.computeRotationScaling(&rotMatrix, &scaleMatrix);
	d->sceneUbo->writeGS(Eigen::Vector2f(d->width, d->height), scaleMatrix(0,0));
}

void XScene::createUbo()
{
    if (!d->sceneUbo) {
        d->sceneUbo = makeShareDbObject<SceneUbo>();
        d->sceneUbo->Create();
    }
}

void XScene::render()
{
    render3D();
    if(isScreenRender)
        render2D();
}

std::vector<sptr<XUCharArray2D>> XScene::renderFbo(int fboWidth, int fboHeight, bool depthEnable, bool multisampleEnable, int samples, int AttachMode)
{
    std::vector<sptr<XUCharArray2D>> result;
    //创建FBO
    fboWidth = getViewportWidth();
    fboHeight = getViewportHeight();
    makeCurrent();
    auto fbo = makeShareDbObject<XOpenGLFramebufferObject>();
    fbo->create();
    fbo->bind();

    fbo->setWidth(/*getViewportWidth()*/fboWidth);
    fbo->setHeight(/*getViewportHeight()*/fboHeight);

    if (multisampleEnable) {
    XOpenGLFuntion::checkGLError();
        fbo->addAttachmentMSAA(
			XOpenGLFramebufferObject::Attachment::Color,                        //附件类型
			XOpenGLTexture::TextureFormat::RGBA8_UNorm,                     //内部格式
			XOpenGLTexture::PixelFormat::RGBA,                                          //外部数据格式
			XOpenGLTexture::PixelType::UInt8
        );
        XOpenGLFuntion::checkGLError();
    }
    else {
		fbo->addAttachment(
			XOpenGLFramebufferObject::Attachment::Color,                        //附件类型
			XOpenGLTexture::TextureFormat::RGBA8_UNorm,                     //内部格式
			XOpenGLTexture::PixelFormat::RGBA,                                          //外部数据格式
			XOpenGLTexture::PixelType::UInt8                                               //外部数据类型
		);
    }

	if (depthEnable)
		if (multisampleEnable) {
			fbo->addAttachmentMSAA(
				XOpenGLFramebufferObject::Attachment::Depth,
				XOpenGLTexture::TextureFormat::D32F,
				XOpenGLTexture::PixelFormat::Depth,
				XOpenGLTexture::PixelType::Float32);
		}
		else {
			fbo->addAttachment(
				XOpenGLFramebufferObject::Attachment::Depth,
				XOpenGLTexture::TextureFormat::D32F,
				XOpenGLTexture::PixelFormat::Depth,
				XOpenGLTexture::PixelType::Float32
			);
		}

    auto complete_flag =fbo->isComplete();
    isScreenRender = false;
    render();
    isScreenRender = true;
    makeCurrent();
    fbo->release();

	{
		//读取颜色纹理数据
		auto texture = fbo->getColorAttachment();
		texture->bind();

        sptr<XOpenGLBuffer> pbo;
       
		if (multisampleEnable) {
            pbo = texture->mapMultiSampleColor(fbo->getId());
		}
		else {
            pbo = texture->map();
		}

        void *p = pbo->map(XOpenGLBuffer::Access::ReadOnly);
		auto data = makeShareDbObject<XUCharArray2D>();
		data->setComponent(4);
		data->setDimensions(fboHeight, fboWidth);
		memcpy(data->data(0, 0), p, fboWidth*fboHeight * 4);
		
		texture->release();
        result.push_back(data);
	}

	if (depthEnable)
	{
		//读取深度纹理数据
		auto texture = fbo->getDepthAttachment();
		texture->bind();

        sptr<XOpenGLBuffer> pbo;
		if (depthEnable) {
			if (multisampleEnable == true) {
                pbo = texture->mapMultiSampleDepth(fbo->getId());
			}
			else {
                pbo = texture->map();
			}
		}
        void* p = pbo->map(XOpenGLBuffer::Access::ReadOnly);
		auto data = makeShareDbObject<XUCharArray2D>();
		data->setComponent(1);
		data->setDimensions(fboHeight,fboWidth);

        std::vector<float> depthData(fboWidth*fboHeight);
        for (int i = 0; i < fboHeight * fboWidth; i++) {
            data->data(0, 0)[i] = ((float*)p)[i]*255;
            depthData[i] = ((float*)p)[i];
        }
		
		texture->release();
		result.push_back(data);
	}
    
    doneCurrent();
    return result;
}

void XScene::render3D()
{
	makeCurrent();

    auto glEnableObj = makeShareDbObject<XOpenGLEnable>();

    d->createGrid();

    d->createAxisShape();

    {
		d->createViewSelection3D();

		d->viewSelection->update(d->shapes, d->camera);

		glEnableObj->enable(XOpenGLEnable::EnableType::DEPTH_TEST);             //启用深度测试
		//glEnableObj->enable(XOpenGLEnable::EnableType::SCISSOR_TEST);       //启用剪裁测试
		glEnableObj->enable(XOpenGLEnable::EnableType::MULTISAMPLE);          //启用多采样抗锯齿

		// 设置裁剪区域与视口一致
		glEnable(GL_SCISSOR_TEST);
		glScissor(d->startx, d->starty, d->width, d->height); // 与 glViewport 设置相同

		// 现在清除操作只会影响裁剪区域
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		auto fbo = d->viewSelection->getFbo(0);

		fbo->getColorAttachment()->bind();
		fbo->getColorAttachment()->bindUnit(2);

		auto shader = d->shaderManger->getShader3D(PrimitveType::triangle);
		shader->use();

		updateUbo2GPU();
		//绘制之前手动更新视口
		updateViewport();


		for (auto shape : d->shapes) {
			shape->draw();
		}



		fbo->getColorAttachment()->release();
		//glEnableObj->restore();
		shader->unUse();
	}

    #if 1
    {
        //绘制网格
        glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto shader = d->shaderManger->getGridShader3D();
		shader->use();
        Eigen::Affine3f t= Eigen::Affine3f::Identity();
        t.rotate(Eigen::AngleAxisf(XQ::Matrix::radian(90), Eigen::Vector3f::UnitX()));
        Eigen::Matrix4f mat = t.matrix();
        //mat = Eigen::Matrix4f::Identity();
        shader->setMat4("gridPlaneMatInWorld",mat);
		auto shape = d->gridShape;
		shape->setPolygonMode(PolygonMode::fill);
		shape->draw(shader);
		shape->setPolygonMode(PolygonMode::fill);
		shader->unUse();

        glEnableObj->restore();
    }
#endif

if(isScreenRender)
    {
		auto shape = d->axisShape;
        d->viewSelection->update({ shape }, d->camera);

		//绘制坐标轴
		glEnableObj->enable(XOpenGLEnable::EnableType::DEPTH_TEST);
		glEnableObj->enable(XOpenGLEnable::EnableType::MULTISAMPLE);       //启用多采样抗锯齿
		glClear(GL_DEPTH_BUFFER_BIT);

		auto worldPos = d->camera->ComputeDisplayToWorld(Eigen::Vector3f(30.f/(float)d->width, 30/(float)d->height, 0.5));
		shape->setPosition(worldPos.x(), worldPos.y(), worldPos.z());

        auto z = d->camera->ComputeWorldToCamera(worldPos).z();
        auto scale = d->camera->scaleFactorH(z,d->width);  //相机宽度：屏幕宽度

        auto scaleFactor = 30. * scale / 2.;
        shape->setScale(scaleFactor, scaleFactor, scaleFactor);
		auto shader = d->shaderManger->getShader3D(PrimitveType::triangle);

        
		auto fbo = d->viewSelection->getFbo(0);
		fbo->getColorAttachment()->bind();
		fbo->getColorAttachment()->bindUnit(2);

		shader->use();
		shape->setPolygonMode(PolygonMode::fill);
		shape->draw();
		shader->unUse();
        fbo->getColorAttachment()->release();

        glEnableObj->restore();
    }

    
	doneCurrent();
}

void XScene::render2D()
{
    makeCurrent();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	{
        if (!d->fontTexture) {
			auto texture = makeShareDbObject<XOpenGLTexture>();

			texture->setTarget(XOpenGLTexture::Target::Target2DArray);
			texture->create();
			texture->bind();
			texture->setMinificationFilter(XOpenGLTexture::Filter::Linear);
			texture->setMagnificationFilter(XOpenGLTexture::Filter::Linear);
			//texture->setWrapMode(XOpenGLTexture::CoordinateDirection::DirectionS, XOpenGLTexture::WrapMode::ClampToEdge);
			//texture->setWrapMode(XOpenGLTexture::CoordinateDirection::DirectionT, XOpenGLTexture::WrapMode::ClampToEdge);
    
            d->fontTexture = texture;
            d->fontTexture->setInternalFormat(XOpenGLTexture::TextureFormat::RGB8_UNorm);
			d->fontTexture->bind();
			d->fontTexture->bindUnit(6);
			d->fontTexture->release();
        }
        else {
            //纹理存在，但是可能数据尚未加载，因此需要重新加载数据
			if (d->fontinitialized == false) {

				d->result_future = std::async(std::launch::async, []() {
					int width = 0;
					int height = 0;
					std::tuple<int, int, std::vector<const void*>> result;
					(&std::get<2>(result))->resize(11);

					auto fn = [&result, &width, &height](int i) {
						std::string  str = std::to_string(i).append(".bmp");
						auto info = stbImage::readPicture(XQ::ShareVar::instance().currentExeDir + "\\sdf\\" + str, false);
						width = info.width;
						height = info.height;
						std::get<2>(result)[i] = info.data;
						};

					XQ::ParaAlgo::ParallelForeach(0, 11, fn, 1);

					std::get<0>(result) = width;
					std::get<1>(result) = height;
					return result;
					});

				d->fontinitialized = true;
			}
				//判断任务是否完成
				if (d->result_future.valid() &&  d->result_future.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready) {
					auto datas = d->result_future.get();
					d->fontTexture->bind();
					d->fontTexture->setData(std::get<0>(datas), std::get<1>(datas), 0, XOpenGLTexture::PixelFormat::RGB, XOpenGLTexture::PixelType::UInt8, std::get<2>(datas));
					d->fontTexture->release();

                    //内存释放
                    for (auto data : std::get<2>(datas)) {
                        stbImage::freeImageData((void*)data);
                    }
					std::cout << "font texture data loaded" << std::endl;
				}	
        }
	}

    d->createGrid2d();

    auto glEnableObj = makeShareDbObject<XOpenGLEnable>();

    //!
    //![1] openGL设置
    {
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glEnableObj->enable(XOpenGLEnable::EnableType::MULTISAMPLE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
	
	//!
    //![2] 更新视口以及UBO缓冲
	{
		updateUbo2GPU();
		updateViewport();
	}


	//!
	//![4] 绘制屏幕网格
	{
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		auto shader = d->shaderManger->getGridShader2D();
		auto shape = d->gridShape2d;
		shape->draw(Eigen::Matrix4f::Identity());
		glEnableObj->restore();
	}

	//!
    //![3] 绘制图元
    
    glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);
    {
		for (auto grapicsItem : d->shapes2D) {
             grapicsItem->setSceneMatrix(d->sceneFrameInVirtualWorld.matrix());
             grapicsItem->draw(Eigen::Matrix4f::Identity());
		}
    }
    
    glEnableObj->restore();

    doneCurrent();
}

void XScene::updateVirtualWorldFrame()
{
    //虚拟世界坐标系的原点在相机坐标系的近平面与相机Z轴的交点上
    // 虚拟坐标系的X Y Z轴与相机坐标系方向相同，但是X Y轴进行了缩放
	//因此欲得到该坐标系，只需要拿着相机坐标系进行变换即可

	auto camera = d->camera;
	auto matrix1 = camera->getViewMatrix();   //相机的view矩阵
	auto matrix = matrix1.inverse();					//当前相机的位姿

	//按照包围盒缩放物体
	d->virtualWorldFrame = Eigen::Affine3f::Identity();
	d->virtualWorldFrame.matrix() = matrix;

	// SCALEX = ( 相机宽度) / 屏幕宽度;
	// SCALEY = ( 相机高度) / 屏幕高度;

	auto sx = camera->scaleFactorHZnear(getViewportWidth());
	auto sy = camera->scaleFactorVZnear(getViewportHeight());
		

	d->virtualWorldFrame.scale(Eigen::Vector3f(sx, sy, 1));
	d->virtualWorldFrame.translate(Eigen::Vector3f(0, 0, -(camera->getZnear() * (1 + 0.01))));
}

void XScene::resizeEvent(int x, int y, int w, int h)
{
    if(d->camera)
        d->camera->setAspect((float)w / (float)h);

    makeCurrent();
    setViewport(x,y,w,h);
	
    if(d->viewSelection)
	    d->viewSelection->updateBufferSize(w, h);
        
    if(d->viewSelection2D)
	    d->viewSelection2D->updateBufferSize(w, h);
    doneCurrent();
}

void XScene::mousePressEvent(int x, int y, int type)
{
  /*  auto selectData = getPointSelection(x,y);
    if(selectData.objectId!= 0)
	    std::cout << std::dec << "鼠标选中:  object " << selectData.objectId << " primitiveID " << selectData.primitiveId << "" << std::endl;*/
}

void XScene::mouseDoublePressEvent(int x, int y, int type)
{
    if (type == /*(int)Qt::LeftButton*/1) {
        //获取当前选中的物体
		makeCurrent();
		d->createViewSelection3D();

		d->viewSelection->update({d->axisShape}, d->camera);
		auto selectData = d->viewSelection->getPointSelection(x - d->startx, y - d->starty, d->width, d->height,0);
		doneCurrent();
        selectData.objectId;
        if (selectData.objectId!= 0) {
            std::cout << std::dec << "鼠标双击:  object " << selectData.objectId << " primitiveID " << selectData.primitiveId << "" << std::endl;
            //获取面片的法向，然后让相机指向该法向
            auto shape = d->axisShape;
            if (shape) {        
                auto normal = shape->getInput()->getFaceNormal(selectData.primitiveId);
                d->camera->setEyeDir(normal);
            }
        }
    }
}

void XScene::mouseMoveEvent(int x,int y)
{
    if(!isBelongtoViewPort(x,y))
        return;
    setMousePos(x,y);
    /*auto selectData = getPointSelection(x, y);
        if (selectData.objectId != 0)
            std::cout << std::dec << "鼠标预选:  object " << selectData.objectId << " primitiveID " << selectData.primitiveId << "" << std::endl;*/
}

void XScene::mouseReleaseEvent(int x, int y, int type)
{
}

void XScene::wheelEvent(int angle,int x,int y)
{
    if(!isBelongtoViewPort(x,y))
        return;
	double factor = 1;
    float t = angle;
	if (!std::signbit(t)) {
		factor = 1.1;
	}
	else {
		factor = 1. / 1.1;
	}
	d->camera->scale(factor);

    //场景坐标系下的缩放
    auto center = screenPos2ScenePos(XQ::Vec2u(x,y));
    
    //控制缩放范围

    auto transform_data = XQ::Matrix::transformDecomposition_TRS(d->sceneFrameInVirtualWorld);
    if (transform_data.sx*factor > 0.1 && transform_data.sx *factor <50) {
        d->sceneFrameInVirtualWorld.translate(Eigen::Vector3f(center.x(), center.y(), 0));
        d->sceneFrameInVirtualWorld.scale(Eigen::Vector3f(factor, factor, 1));
        d->sceneFrameInVirtualWorld.translate(Eigen::Vector3f(-center.x(), -center.y(), 0));
    }
}

void XScene::fitView2D()
{
   auto data = XQ::Matrix::transformDecomposition_TRS(d->sceneFrameInVirtualWorld);
   data.sx = 1;
   data.sy = 1;
   data.sz = 1;
   data.tx = 0;
   data.ty = 0;
   data.tz = 0;

   auto mat = XQ::Matrix::computeMatrix(data);
   d->sceneFrameInVirtualWorld.matrix() = mat;
}

void XScene::setGrid2dVisible(bool visible)
{
    if (d->gridShape2d) {
        d->gridShape2d->setVisible(visible);
    }
}

bool XScene::isGrid2dVisible()
{
    if(d->gridShape2d)
        return d->gridShape2d->isVisible();
    return false;
}

void XScene::setGrid3dVisible(bool visible)
{
    if (d->gridShape) {
        d->gridShape->setVisible(visible);  
     }
}

bool XScene::isGrid3dVisible()
{
	if (d->gridShape)
		return d->gridShape->isVisible();
	return false;
}

void XScene::setAxis3dVisible(bool visible)
{
    if (d->axisShape) {
        return d->axisShape->setVisible(visible);
    }
}

void XScene::fitView3D()
{
	auto boundbox = computeBoundBox();
	getCamera()->resetCamera((double*)&boundbox);
}

void XScene::rotate(int x1,int y1,int x2,int y2)
{
    if(!isBelongtoViewPort(x2,y2))
        return;
	Eigen::Vector2f curPos_(x1 - d->startx, y1 - d->starty);
	Eigen::Vector2f lastPos_(x2 - d->startx, y2 - d->starty);
    d->camera->transformTrackball(curPos_,lastPos_ ,getViewportWidth(), getViewportHeight(), true,true);
}

void XScene::rotate(XQ::Vec2u curpos, XQ::Vec2u lastpos)
{
    return rotate(curpos.x(), curpos.y(), lastpos.x(), lastpos.y());
}

void XScene::translate(int x1, int y1, int x2, int y2)
{
	if (!isBelongtoViewPort(x2, y2))
		return;
	Eigen::Vector2f curPos_(x1-d->startx, y1-d->starty);
	Eigen::Vector2f lastPos_(x2 - d->startx, y2 - d->starty);
	d->camera->transformTrackball(curPos_, lastPos_, getViewportWidth(), getViewportHeight(), false, true);

    //场景坐标系下的平移
	auto cur_scenePos = screenPos2ScenePos(XQ::Vec2u(x1, y1));
	auto last_scenePos = screenPos2ScenePos(XQ::Vec2u(x2, y2));
    d->sceneFrameInVirtualWorld.translate(Eigen::Vector3f(cur_scenePos.x() - last_scenePos.x(), cur_scenePos.y() - last_scenePos.y(), 0));
}

void XScene::translate(XQ::Vec2u curpos, XQ::Vec2u lastpos)
{
    translate(curpos.x(), curpos.y(), lastpos.x(), lastpos.y());
}

XQ::Vec2i XScene::screenPos2ViewportPos(XQ::Vec2u pos) const
{
    XQ ::Vec2i result;
    result.x() = pos.x() - d->startx;
    result.y() = pos.y() - d->starty;
    return result;
}

XQ::Vec2u XScene::viewportPos2ScreenPos(XQ::Vec2i pos) const
{
    XQ::Vec2u result;
    result.x() = pos.x() + d->startx;
    result.y() = pos.y() + d->starty;
    return result;
}

//场景坐标系下的点转化到场景视口坐标系下的点
XQ::Vec2i XScene::scenePos2ViewportPos(XQ::Vec2f pos) const
{
    Eigen::Vector3f posInVirtualWorld = d->sceneFrameInVirtualWorld*Eigen::Vector3f(pos.x(), pos.y(), 0);
	auto x = posInVirtualWorld.x();
	auto y = posInVirtualWorld.y();

    return XQ::Vec2i( x + 0.5*getViewportWidth(), y + 0.5*getViewportHeight());
}

//视口坐标系在左下角
XQ::Vec2f XScene::viewportPos2ScenePos(XQ::Vec2i pos) const
{
    pos.x() -= 0.5 * getViewportWidth();
    pos.y() -= 0.5 * getViewportHeight();

    Eigen::Vector3f posInSceneFrmae = d->sceneFrameInVirtualWorld.inverse()*Eigen::Vector3f(pos.x(), pos.y(), 0);

    return XQ::Vec2f(posInSceneFrmae.x(), posInSceneFrmae.y());
}

XQ::Vec2f XScene::screenPos2ScenePos(XQ::Vec2u pos) const
{
    //屏幕到视口
    auto viewportPos = screenPos2ViewportPos(pos);

    //视口到场景
    auto scenePos = viewportPos2ScenePos(viewportPos);

    return scenePos;
}

XQ::Vec2u XScene::scenePos2ScreenPos(XQ::Vec2f pos) const
{
    //场景到视口
    auto viewportPos = scenePos2ViewportPos(pos);

    //视口到屏幕
    auto screenPos = viewportPos2ScreenPos(viewportPos);
    return screenPos;
}

Eigen::Matrix4f XScene::sceneScreenPos2ScenePos()
{
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();
    auto scene_w = getViewportWidth();
    auto scene_h = getViewportHeight();

    transform.translate(Eigen::Vector3f(-0.5 * scene_w, -0.5 * scene_h, 0));

    Eigen::Matrix4f screen2viewPort = transform.matrix();

    return d->sceneFrameInVirtualWorld.inverse() * screen2viewPort;
}

Eigen::Matrix4f XScene::sceneScreenPos2ScenePosMat(std::shared_ptr<DataBaseObject> scene)
{
    if (auto s = std::dynamic_pointer_cast<XScene>(scene)) {
        return s->sceneScreenPos2ScenePos();
    }
}

bool XScene::isBelongtoViewPort(int x, int y)
{
    if(x >= d->startx && x < d->startx + d->width && y >= d->starty && y < d->starty + d->height)
        return true;
    return false;
}

void XScene::setMousePos(int x, int y)
{
	mousePosx = x;
	mousePosy = y;
}

void XScene::makeCurrent()
{
	if (d->context)
		d->context->makeCurrent();
}

void XScene::swapBuffers()
{
	if (d->context)
		d->context->swapBuffers();
}

void XScene::doneCurrent()
{
	if (d->context)
		d->context->doneCurrent();
}

XQ::BoundBox  XScene::computeBoundBox() {
	 constexpr double limitMax = std::numeric_limits<double>::max();
     constexpr double limitMin = std::numeric_limits<double>::lowest();;
    XQ::BoundBox boundBox{ limitMax ,limitMax ,limitMax ,limitMin,limitMin,limitMin };
    for (auto& shape : d->shapes) {
        auto shapeBoundBox = shape->getBoundBox();
        boundBox.xmin = std::min(boundBox.xmin, shapeBoundBox.xmin);
        boundBox.xmax = std::max(boundBox.xmax, shapeBoundBox.xmax);
        boundBox.ymin = std::min(boundBox.ymin, shapeBoundBox.ymin);
        boundBox.ymax = std::max(boundBox.ymax, shapeBoundBox.ymax);
        boundBox.zmin = std::min(boundBox.zmin, shapeBoundBox.zmin);
        boundBox.zmax = std::max(boundBox.zmax, shapeBoundBox.zmax);
    }

    if (d->shapes.size() == 0) {
        boundBox.xmin = -1;
        boundBox.xmax = 1;
        boundBox.ymin = -1;
        boundBox.ymax = 1;
        boundBox.zmin = -1;
        boundBox.zmax = 1;
    }
    return boundBox;
}

void workThread(/*GLsync sync*/XOpenGL::GlSyncObject syncObject, 
        std::shared_ptr<XOpenGLShareContext> context, 
        std::shared_ptr<XOpenGLBuffer> pbo,
        int width,
        int height
        ) {
    if (!context->makeCurrent()) {
        return;
    }

	auto waitResult = XOpenGLFuntion::xglClientWaitSync(syncObject, XOpenGL::SyncFlags::none, std::numeric_limits<unsigned long long>::max());
	if (waitResult == XOpenGL::SyncStatus::SyncStatusAlreadySignaled ||
		waitResult == XOpenGL::SyncStatus::SyncStatusConditionSatisfied)
	{
		//glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &currentPBO);
		pbo->bind();
		void* d = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		unsigned char* dd = (unsigned char*)d;
	}
	else {
		std::cout << "wait sync failed\n";
	}

   XOpenGLFuntion::xglDeleteSync(syncObject);
   
    context->doneCurrent();
}

std::shared_ptr<XUCharArray2D> XScene::grabFramebuffer()
{
	makeCurrent();
	auto data = makeShareDbObject<XUCharArray2D>();
	data->setComponent(4);
    data->setDimensions(getViewportWidth(), getViewportHeight());

    XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer,0);

    auto pbo =makeShareDbObject< XOpenGLBuffer>();
    pbo->setBufferType(XOpenGLBuffer::PixelPackBuffer);
    pbo->setUsagePattern(XOpenGLBuffer::UsagePattern::StreamRead);
    pbo->create();
    pbo->bind();
    pbo->allocate(getViewportWidth()* getViewportHeight()*4);

    XOpenGLFuntion::xglReadPixels(0, 0, getViewportWidth(), getViewportHeight(),XOpenGL::TextureExternalFormat::BGRA, XOpenGL::DataType::unsigned_byte, nullptr);

    //GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    auto glSyncObject = XOpenGLFuntion::xglFenceSync(XOpenGL::SyncFlags::SyncFlushCommandsBit);

    std::thread t(std::bind(workThread, glSyncObject,d->context->createOrgetShareContext(),pbo, getViewportWidth(),getViewportHeight()));
    t.detach();

	doneCurrent();
	return data;
}

std::shared_ptr<XUCharArray2D> XScene::grabFramebuffer(
	int startx,
	int starty,
	int width,
	int height,
	int dest_x,
	int dest_y,
	int dest_width,
	int dest_height) 
  { 
    makeCurrent();
    auto data = makeShareDbObject<XUCharArray2D>();
    data->setComponent(4);
    
    XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer,0);
	GLint sampleBuffers, samples;

	XOpenGLFuntion::xglGetIntegerv(XOpenGL::OtherType::sample_buffers, sampleBuffers);
	XOpenGLFuntion::xglGetIntegerv(XOpenGL::OtherType::samples, samples);

	if (sampleBuffers && samples > 1) {
		auto resoveFbo = makeShareDbObject<XOpenGLFramebufferObject>();
		resoveFbo->create();
		resoveFbo->bind();
		resoveFbo->addAttachment(XOpenGLFramebufferObject::Attachment::Color, XOpenGLTexture::TextureFormat::RGBA8_UNorm, XOpenGLTexture::PixelFormat::RGBA, XOpenGLTexture::PixelType::UInt8, 0);
		bool flag = resoveFbo->isComplete();
		resoveFbo->release();

		XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::readBuffer, 0);
		XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::drawBuffer, resoveFbo->getId());

		XOpenGLFuntion::xglBlitFramebuffer(0, 0,  width, height, 0, 0, width, height,
			XOpenGL::FlagBits::color_buffer_bit, XOpenGL::FilterType::nearest);
    }

     width =std::min(width, getViewportWidth() - startx);
     height = std::min(height , getViewportHeight() - starty);

     //目标区域总大小
    data->setDimensions(dest_height,dest_width);

	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, dest_x);
	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, dest_y);
	XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, dest_width);

    XOpenGLFuntion::xglReadPixels(startx, starty, width, height, XOpenGL::TextureExternalFormat::RGBA, XOpenGL::DataType::unsigned_byte, data->data(0,0));

    //恢复
    XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, 0);
    XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, 0);
    XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, 0);

    XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer, 0);
    doneCurrent();
    return data;
}