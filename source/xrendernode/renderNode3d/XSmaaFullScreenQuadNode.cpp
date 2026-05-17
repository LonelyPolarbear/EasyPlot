#include "XSmaaFullScreenQuadNode.h"
#include <lib01_shader/xshaderManger.h>
#include "lib01_shader/XBaseShader.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "lib04_opengl/XOpenGLFuntion.h"
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib04_opengl/XOpenGLBuffer.h"
#include "lib04_opengl/XOpenGLEnable.h"
#include "sharevar/XShareVar.h"
#include "lib03_stbImage/stbImage.h"
#include <base/xbaserender/baseRender/XBaseRender.h>

class XSmaaFullScreenQuadNode::Internal {
	public:
		sptr<XOpenGLTexture> smaa_screen_color_texture;					//输入，场景的渲染结果
		sptr<XOpenGLTexture> smaa_screen_depth_texture;					//输入，场景的渲染结果

		sptr<XOpenGLTexture> smaa_edge_texture;					//边缘检测pass输出的纹理

		sptr<XOpenGLTexture> smaa_area_texture;
		sptr<XOpenGLTexture> smaa_search_texture;

		sptr<XOpenGLTexture> smaa_blend_texture;					//混合pass输出的纹理
		//三个着色器
		sptr<xshader> smaa_edge_shader;									//输入原始图像/深度 输出smaa_edge_texture
		sptr<xshader> smaa_blend_shader;									//输入smaa_edge_texture, smaa_area_texture , smaa_search_texture 输出 smaa_blend_texture
		sptr<xshader> smaa_final_shader;									//输入smaa_blend_texture

		sptr<XOpenGLFramebufferObject> smaa_edge_fbo;
		sptr<XOpenGLFramebufferObject> smaa_blend_fbo;
		//sptr<XOpenGLFramebufferObject> smaa_final_fbo;

		bool isShaderInit = false;
		bool isTexInit = false;

		void createFbo(int width,int height) {
			if(isTexInit)
				return;
			isTexInit = true;
			{
				auto areaTexPath = XShareVar::instance().currentExeDir + "/textures/" + "AreaTexDX10.dds";
				auto searchTexPath = XShareVar::instance().currentExeDir + "/textures/" + "SearchTex.png";
				auto areaTexId = stbImage::loadTexture2D(areaTexPath,2);
				auto searchTexId = stbImage::loadTexture2D(searchTexPath,1);
				smaa_area_texture = makeShareDbObject<XOpenGLTexture>();
				smaa_area_texture->createFromExternal(areaTexId);
				smaa_area_texture->bind();
				smaa_area_texture->setMagnificationFilter(XOpenGLTexture::Linear);
				smaa_area_texture->setMinificationFilter(XOpenGLTexture::Linear);
				smaa_area_texture->setWrapMode(XOpenGLTexture::DirectionS, XOpenGLTexture::ClampToEdge);
				smaa_area_texture->setWrapMode(XOpenGLTexture::DirectionT, XOpenGLTexture::ClampToEdge);
				smaa_area_texture->setSwizzleMask(XOpenGLTexture::SwizzleRed, XOpenGLTexture::SwizzleValue::GreenValue);
				smaa_area_texture->setSwizzleMask(XOpenGLTexture::SwizzleGreen, XOpenGLTexture::SwizzleValue::RedValue);
				smaa_area_texture->setExternalFormat(XOpenGLTexture::RGB,XOpenGLTexture::PixelType::UInt8);
				smaa_area_texture->release();
				//auto ss = smaa_area_texture->map2pbo()->map2cpu();

				smaa_search_texture = makeShareDbObject<XOpenGLTexture>();
				smaa_search_texture->createFromExternal(searchTexId);
				smaa_search_texture->bind();
				smaa_search_texture->setMagnificationFilter(XOpenGLTexture::Linear);
				smaa_search_texture->setMinificationFilter(XOpenGLTexture::Linear);
				smaa_search_texture->setWrapMode(XOpenGLTexture::DirectionS, XOpenGLTexture::ClampToEdge);
				smaa_search_texture->setWrapMode(XOpenGLTexture::DirectionT, XOpenGLTexture::ClampToEdge);
				smaa_search_texture->release();
			}

			{
				smaa_edge_fbo = makeShareDbObject<XOpenGLFramebufferObject>();
				smaa_edge_texture = makeShareDbObject<XOpenGLTexture>();
				smaa_edge_texture->setInternalFormat(XOpenGLTexture::RGBA8_UNorm);
				
				smaa_edge_texture->create();
				smaa_edge_texture->bind();
				smaa_edge_texture->setMagnificationFilter(XOpenGLTexture::Nearest);
				smaa_edge_texture->setMinificationFilter(XOpenGLTexture::Nearest);
				smaa_edge_texture->release();
				smaa_edge_fbo->create();
				smaa_edge_fbo->bind();
				smaa_edge_fbo->addAttachment(XOpenGLFramebufferObject::Color,smaa_edge_texture,0);		//不需要深度附件
				smaa_edge_fbo->addAttachment(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, XOpenGLTexture::TextureFormat::D24S8, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8);	//深度模板附件
				smaa_edge_fbo->release();
				smaa_edge_fbo->updateBufferSize(width,height);
			}

			{
				smaa_blend_fbo = makeShareDbObject<XOpenGLFramebufferObject>();
				smaa_blend_texture = makeShareDbObject<XOpenGLTexture>();
				smaa_blend_texture->setInternalFormat(XOpenGLTexture::RGBA8_UNorm);
				smaa_blend_texture->create();
				smaa_blend_fbo->create();
				smaa_blend_fbo->bind();
				smaa_blend_fbo->addAttachment(XOpenGLFramebufferObject::Color, smaa_blend_texture, 0);		//不需要深度附件
				smaa_blend_fbo->addAttachment(XOpenGLFramebufferObject::Attachment::CombinedDepthStencil, XOpenGLTexture::TextureFormat::D24S8, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8);	//深度模板附件
				smaa_blend_fbo->release();
				smaa_blend_fbo->updateBufferSize(width, height);
			}

			
		}

		void createShader() {
			if (isShaderInit) {
				return;
			}
			isShaderInit = true;
			{
				smaa_edge_shader = makeShareDbObject<xshader>();
				smaa_edge_shader->create();
				auto vs_id = smaa_edge_shader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa.vs");
				auto fs_id = smaa_edge_shader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa_edge.fs");
				smaa_edge_shader->link({ vs_id,fs_id });
			}
			{
				smaa_blend_shader = makeShareDbObject<xshader>();
				smaa_blend_shader->create();
				auto vs_id = smaa_blend_shader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa.vs");
				auto fs_id = smaa_blend_shader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa_blend.fs");
				smaa_blend_shader->link({ vs_id,fs_id });
			}
			{
				smaa_final_shader = makeShareDbObject<xshader>();
				smaa_final_shader->create();
				auto vs_id = smaa_final_shader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa.vs");
				auto fs_id = smaa_final_shader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa_final.fs");
				smaa_final_shader->link({ vs_id,fs_id });
			}
		}
};

XSmaaFullScreenQuadNode::XSmaaFullScreenQuadNode():mData(new Internal)
{

}

XSmaaFullScreenQuadNode::~XSmaaFullScreenQuadNode()
{
}

void XSmaaFullScreenQuadNode::draw(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	if (AttrEnableSmaa->getValue()) {
		drawSmaa(render,parentMatrix);
	}
	else {
		drawNoSmaa(render,parentMatrix);
	}
}

void XSmaaFullScreenQuadNode::draw(sptr<XBaseRender> render, std::shared_ptr<xshader>s, const Eigen::Matrix4f& parentMatrix)
{
	XGeometryNode::draw(render, s, parentMatrix);
}

void XSmaaFullScreenQuadNode::Init()
{
	XFullScreenQuadNode::Init();
	XQ_ATTR_ADD_INIT(AttrEnableSmaa,false);
}

void XSmaaFullScreenQuadNode::drawSmaa(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto width = render->getConvertViewPort()[2];
	auto height = render->getConvertViewPort()[3];

	mData->createFbo(width, height);
	mData->createShader();
	mData->smaa_edge_fbo->updateBufferSize(width, height);
	mData->smaa_blend_fbo->updateBufferSize(width, height);

	if (mData->smaa_screen_color_texture) {
		//[1]
#if 1
		mData->smaa_edge_fbo->bind();
		XOpenGLFuntion::xglClearColor((float)0, (float)0, (float)0, (float)0,0);
		//XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::color_buffer_bit);
		mData->smaa_edge_shader->use();
		mData->smaa_screen_color_texture->bindUnit(3);
		mData->smaa_edge_shader->setVec4("SMAA_RT_METRICS", 1.0 / width, 1.0 / height, width, height);
		draw(render, mData->smaa_edge_shader, parentMatrix);
		mData->smaa_edge_fbo->release();

		//[2]
		mData->smaa_blend_fbo->bind();
		XOpenGLFuntion::xglClearColor((float)0, (float)0, (float)0, (float)0, 0);
		//XOpenGLFuntion::xglClear((unsigned int)XOpenGL::BufferBits::color_buffer_bit);
		mData->smaa_blend_shader->use();
		mData->smaa_blend_shader->setVec4("SMAA_RT_METRICS", 1.0 / width, 1.0 / height, width, height);
		mData->smaa_blend_shader->setVec4("subsampleIndices", 0, 0, 0, 0);
		mData->smaa_screen_color_texture->bindUnit(3);
		mData->smaa_area_texture->bindUnit(4);
		mData->smaa_search_texture->bindUnit(5);
		draw(render, mData->smaa_blend_shader, parentMatrix);
		mData->smaa_blend_fbo->release();
#endif
		//[3]

		XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer, 0);
		mData->smaa_final_shader->use();
		mData->smaa_final_shader->setVec4("SMAA_RT_METRICS", 1.0 / (float)width, 1.0 / (float)height, width, height);
		mData->smaa_final_shader->setBool("u_enableSmaa",AttrEnableSmaa->getValue());
		mData->smaa_screen_color_texture->bindUnit(3);
		mData->smaa_area_texture->bindUnit(4);
		mData->smaa_search_texture->bindUnit(5);
		mData->smaa_blend_texture->bindUnit(6);
		mData->smaa_edge_texture->bindUnit(7);
		mData->smaa_final_shader->unUse();
		draw(render, mData->smaa_final_shader, parentMatrix);
	}
}

void XSmaaFullScreenQuadNode::drawNoSmaa(sptr<XBaseRender> render, const Eigen::Matrix4f& parentMatrix)
{
	auto viewport =render->getConvertViewPort();
	auto width = render->getConvertViewPort()[2];
	auto height = render->getConvertViewPort()[3];

	mData->createFbo(width, height);
	mData->createShader();

	if (mData->smaa_screen_color_texture) {

		XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer, 0);
		mData->smaa_final_shader->use();
		mData->smaa_final_shader->setVec4("SMAA_RT_METRICS", 1.0 / (float)width, 1.0 / (float)height, width, height);
		mData->smaa_final_shader->setVec2("viewPortOrigin", (float)viewport[0],(float)viewport[1]);
		mData->smaa_final_shader->setBool("u_enableSmaa", AttrEnableSmaa->getValue());

		mData->smaa_screen_color_texture->bindUnit(3);
		mData->smaa_area_texture->bindUnit(4);
		mData->smaa_search_texture->bindUnit(5);
		mData->smaa_blend_texture->bindUnit(6);
		mData->smaa_edge_texture->bindUnit(7);
		mData->smaa_final_shader->unUse();

		XOpenGLFuntion::xglBindFramebuffer(XOpenGL::FrameBufferType::framebuffer, 0);
		draw(render, mData->smaa_final_shader, parentMatrix);
	}
}

void XSmaaFullScreenQuadNode::setInputColorTexture(sptr<XDataBaseObject> colorTexture, sptr<XDataBaseObject> depthTexture)
{
	mData->smaa_screen_color_texture = colorTexture->asDerived<XOpenGLTexture>();
	mData->smaa_screen_depth_texture = depthTexture->asDerived<XOpenGLTexture>();
	mData->smaa_screen_color_texture->bind();
	//mData->smaa_screen_color_texture->setMagnificationFilter(XOpenGLTexture::Linear);
	//mData->smaa_screen_color_texture->setMinificationFilter(XOpenGLTexture::Linear);
	mData->smaa_screen_color_texture->release();
}
