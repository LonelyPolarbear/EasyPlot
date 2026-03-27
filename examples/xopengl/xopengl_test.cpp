#include <lib04_opengl/XOpenGLContext.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLFuntion.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>

#include <xtest/XTest.h>
#include <xlog/XLogger.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
//离线渲染环境搭建测试
void TestOffsetWindow() {
	XLOG_DEBUG("TestOffsetWindow");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		// do something here
		context->doneCurrent();
	}
	return;
}

void CreateFbo() {
	XLOG_DEBUG("CreateFbo");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		auto fbo =makeShareDbObject<XOpenGLFramebufferObject>();
		fbo->create();

		fbo->release(XOpenGL::FrameBufferType::framebuffer);
	}
	context->doneCurrent();
	return;
}

/**
 * @brief 对纹理进行测试，测试XOpenGLTexture的所有接口
 */
void TestTexture1D_R8U() {
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		//为纹理分配数据
		auto cpuDataArray =makeShareDbObject<XUCharArray>();
		int num_cpu =20;
		int num_gpu = 10;
		cpuDataArray->setNumOfTuple(num_cpu);

		for (int i = 0; i < num_cpu; i++) {
			cpuDataArray->setTuple(i,i);
		}

		XLOG_DEBUG(u8"[1] 一维纹理原始CPU数据{}{}","\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		tex->setTarget(XOpenGLTexture::Target1D);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R8U);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red_Integer, XOpenGLTexture::PixelType::UInt8);
		tex->texStorage1D(num_gpu);
		
		json obj;
		obj["Target"]="Target1D";
		obj["InternalFormat"] = "R8U";
		obj["pixelFormat"] = "Red_Integer";
		obj["pixelType"] = "UInt8";

		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", obj.dump(2));
		XLOG_INFO("{}textureMetadata:{}{}","\n", "\n", tex->dump());

		XLOG_INFO(u8"[3] 纹理数据上传");
		tex->setSubData1D(0, num_gpu,cpuDataArray->data(0));

		
		auto raw_data =tex->map2pbo()->map2cpu();
		XLOG_INFO(u8"[4] 回读纹理数据{} {}", "\n", raw_data->toString());

		XLOG_INFO(u8"[5] 分配超过纹理容量的数据，测试动态重新创建");
		tex->texStorage1D(num_cpu);

		XLOG_INFO(u8"[6] 纹理数据上传");
		tex->setSubData1D(0, num_cpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[7] 回读纹理数据{} {}", "\n", raw_data->toString());
		}
		
		
	}
	context->doneCurrent();
	return;
}

int main() {
	
	XTestApp app("XOpenGL测试");

	app.addCmd("TestOffsetWindow", "离线渲染环境搭建测试",TestOffsetWindow);
	app.addCmd("CreateFbo", "创建FBO", CreateFbo);
	app.addCmd("TestTexture1D_R8U", "1D纹理的测试", TestTexture1D_R8U);

	return app.run();
}