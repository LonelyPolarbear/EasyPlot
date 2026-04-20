#include <lib04_opengl/XOpenGLContext.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLFuntion.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>

#include <xtest/XTest.h>
#include <xlog/XLogger.h>
#include <nlohmann/json.hpp>
#include <windows.h>

using json = nlohmann::json;

float fract(float t) {
	double int_part; 
	double frac_part = std::modf(t, &int_part); 
	return frac_part;
}
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
		auto cpuDataArray = makeShareDbObject<XUCharArray>();
		int num_cpu = 20;
		int num_gpu = 10;
		cpuDataArray->setNumOfTuple(num_cpu);

		for (int i = 0; i < num_cpu; i++) {
			cpuDataArray->setTuple(i, i);
		}

		XLOG_DEBUG(u8"[1] 一维纹理原始CPU数据{}{}", "\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		tex->setTarget(XOpenGLTexture::Target1D);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R8U);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red_Integer, XOpenGLTexture::PixelType::UInt8);
		tex->texStorage1D(num_gpu);

		json obj;
		obj["Target"] = "Target1D";
		obj["InternalFormat"] = "R8U";
		obj["pixelFormat"] = "Red_Integer";
		obj["pixelType"] = "UInt8";

		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", obj.dump(2));
		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", tex->dump());

		XLOG_INFO(u8"[3] 纹理数据上传");
		tex->setSubData1D(0, num_gpu, cpuDataArray->data(0));


		auto raw_data = tex->map2pbo()->map2cpu();
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

void TestTexture1D_R8_float() {
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		//为纹理分配数据
		auto cpuDataArray = makeShareDbObject<XFloatArray>();
		int num_cpu = 20;
		int num_gpu = 10;
		cpuDataArray->setNumOfTuple(num_cpu);

		for (int i = 0; i < num_cpu; i++) {
			cpuDataArray->setTuple(i, fract( i*0.1));
		}

		XLOG_DEBUG(u8"[1] 一维纹理原始CPU数据{}{}", "\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		tex->setTarget(XOpenGLTexture::Target1D);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R8_UNorm);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red, XOpenGLTexture::PixelType::Float32);
		tex->texStorage1D(num_gpu);

		json obj;
		obj["Target"] = "Target1D";
		obj["InternalFormat"] = "R8_UNorm";
		obj["pixelFormat"] = "Red";
		obj["pixelType"] = "Float32";

		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", obj.dump(2));
		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", tex->dump());

		XLOG_INFO(u8"[3] 纹理数据上传,使用浮点数上传纹理数据");
		tex->setSubData1D(0, num_gpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[4-1] 回读纹理字节数据{} {}", "\n", raw_data->toString());
			auto ss = raw_data->heteroCast<float>(1);
			XLOG_INFO(u8"[4-1] 回读纹理浮点数据{} {}", "\n", ss->toString());
		}
		

		XLOG_INFO(u8"[5] 分配超过纹理容量的数据，测试动态重新创建");
		tex->texStorage1D(num_cpu);

		XLOG_INFO(u8"[6] 纹理数据上传");
		tex->setSubData1D(0, num_cpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[7-1] 回读纹理数据{} {}", "\n", raw_data->toString());
			auto ss = raw_data->heteroCast<float>(1);
			XLOG_INFO(u8"[7-2] 回读纹理浮点数据{} {}", "\n", ss->toString());
		}
	}
	context->doneCurrent();
	return;
}

void TestTexture1D_R8_u8() {
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		//为纹理分配数据
		auto cpuDataArray = makeShareDbObject<XUCharArray>();
		int num_cpu = 20;
		int num_gpu = 10;
		cpuDataArray->setNumOfTuple(num_cpu);

		for (int i = 0; i < num_cpu; i++) {
			cpuDataArray->setTuple(i, i*10);
		}

		XLOG_DEBUG(u8"[1] 一维纹理原始CPU数据{}{}", "\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		tex->setTarget(XOpenGLTexture::Target1D);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R8_UNorm);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red, XOpenGLTexture::PixelType::UInt8);
		tex->texStorage1D(num_gpu);

		json obj;
		obj["Target"] = "Target1D";
		obj["InternalFormat"] = "R8_UNorm";
		obj["pixelFormat"] = "Red";
		obj["pixelType"] = "UInt8";

		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", obj.dump(2));
		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", tex->dump());

		XLOG_INFO(u8"[3] 纹理数据上传,使用浮点数上传纹理数据");
		tex->setSubData1D(0, num_gpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[4-1] 回读纹理字节数据{} {}", "\n", raw_data->toString());
		}


		XLOG_INFO(u8"[5] 分配超过纹理容量的数据，测试动态重新创建");
		tex->texStorage1D(num_cpu);

		XLOG_INFO(u8"[6] 纹理数据上传");
		tex->setSubData1D(0, num_cpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			auto row_data2UInt = raw_data->convert<uint32_t>();
			auto s = row_data2UInt->heteroCast<float>(1);
			XLOG_INFO(u8"[7-1] 回读纹理数据{} {}", "\n", raw_data->toString());
			XLOG_INFO(u8"[7-2] 回读纹理数据 浮点数{} {}", "\n", s->toString());
		}
	}
	context->doneCurrent();
	return;
}

void TestTexture1D_R32U() {
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		//为纹理分配数据
		auto cpuDataArray = makeShareDbObject<XUIntArray>();
		int num_cpu = 20;
		int num_gpu = 10;
		cpuDataArray->setNumOfTuple(num_cpu);

		for (int i = 0; i < num_cpu; i++) {
			cpuDataArray->setTuple(i, i);
		}

		XLOG_DEBUG(u8"[1] 一维纹理原始CPU数据{}{}", "\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		tex->setTarget(XOpenGLTexture::Target1D);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R32U);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red_Integer, XOpenGLTexture::PixelType::UInt32);
		tex->texStorage1D(num_gpu);

		json obj;
		obj["Target"] = "Target1D";
		obj["InternalFormat"] = "R32U";
		obj["pixelFormat"] = "Red_Integer";
		obj["pixelType"] = "UInt32";

		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", obj.dump(2));
		XLOG_INFO("{}textureMetadata:{}{}", "\n", "\n", tex->dump());

		XLOG_INFO(u8"[3] 纹理数据上传");
		tex->setSubData1D(0, num_gpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[4 -1] 回读纹理数据{} {}", "\n", raw_data->toString());

			auto ss = raw_data->heteroCast<uint32_t>(1);
			XLOG_INFO(u8"[7-2] 回读转型后的纹理数据{} {}", "\n", ss->toString());
		}
		

		XLOG_INFO(u8"[5] 分配超过纹理容量的数据，测试动态重新创建");
		tex->texStorage1D(num_cpu);

		XLOG_INFO(u8"[6] 纹理数据上传");
		tex->setSubData1D(0, num_cpu, cpuDataArray->data(0));

		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[7-1] 回读纹理数据{} {}", "\n", raw_data->toString());

			auto ss = raw_data->heteroCast<uint32_t>(1);
			XLOG_INFO(u8"[7-2] 回读转型后的纹理数据{} {}", "\n", ss->toString());
		}
	}
	context->doneCurrent();
	return;
}

void TestTexture1D_Depth() {
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();
	context->create(window->winId());
	if (context->makeCurrent()) {
		{
			XLOG_INFO(u8"[1] 纹理创建");
			int num_gpu = 5;
			auto tex = makeShareDbObject<XOpenGLTexture>();
			tex->setTarget(XOpenGLTexture::Target1D);
			tex->create();
			tex->bind();
			tex->setInternalFormat(XOpenGLTexture::TextureFormat::D32);
			tex->setExternalFormat(XOpenGLTexture::PixelFormat::Depth, XOpenGLTexture::PixelType::Float32);
			tex->texStorage1D(num_gpu);

			std::vector<float> cpuData{ 0.1,0.2,0.3,0.4,0.5 };
			XLOG_INFO(u8"[2] 纹理数据上传");
			tex->setSubData1D(0, num_gpu, cpuData.data());

			auto raw_data = tex->map2pbo()->map2cpu();
			auto s = raw_data->heteroCast<float>(1);
			XLOG_INFO(u8"[3] 回读纹理数据{} {}", "\n", s->toString());
		}
		{
			XLOG_INFO(u8"[1] 纹理创建");
			int num_gpu = 5;
			auto tex = makeShareDbObject<XOpenGLTexture>();
			tex->setTarget(XOpenGLTexture::Target1D);
			tex->create();
			tex->bind();
			tex->setInternalFormat(XOpenGLTexture::TextureFormat::D32);
			tex->setExternalFormat(XOpenGLTexture::PixelFormat::Depth, XOpenGLTexture::PixelType::UInt32);
			tex->texStorage1D(num_gpu);

			std::vector<uint32_t> cpuData{ 4294967295,200,12368,2147483647 ,888888};
			XLOG_INFO(u8"[2] 纹理数据上传");
			tex->setSubData1D(0, num_gpu, cpuData.data());

			auto raw_data = tex->map2pbo()->map2cpu();
			auto s = raw_data->heteroCast<uint32_t>(1);
			XLOG_INFO(u8"[3] 回读纹理数据{} {}", "\n", s->toString());
		}

		{
			XLOG_INFO(u8"[1] 纹理创建");
			int num_gpu = 5;
			auto tex = makeShareDbObject<XOpenGLTexture>();
			tex->setTarget(XOpenGLTexture::Target1D);
			tex->create();
			tex->bind();
			tex->setInternalFormat(XOpenGLTexture::TextureFormat::D24S8);
			tex->setExternalFormat(XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8);
			tex->texStorage1D(num_gpu);

			std::vector<uint32_t> cpuData{ 0xff123456,200,12368,2147483647 ,888888 };
			XLOG_INFO(u8"[2] 纹理数据上传");
			tex->setSubData1D(0, num_gpu, cpuData.data());

			auto raw_data = tex->map2pbo()->map2cpu();
			auto s = raw_data->heteroCast<uint32_t>(1);
			XLOG_INFO(u8"[3] 回读纹理数据{} {}", "\n", s->toString());
		}

		{
			{
				XLOG_INFO(u8"[1] 纹理创建");
				int num_gpu = 5;
				auto tex = makeShareDbObject<XOpenGLTexture>();
				tex->setTarget(XOpenGLTexture::Target1D);
				tex->create();
				tex->bind();
				tex->setInternalFormat(XOpenGLTexture::TextureFormat::S8);
				tex->setExternalFormat(XOpenGLTexture::PixelFormat::Stencil, XOpenGLTexture::PixelType::UInt8);
				tex->texStorage1D(num_gpu);

				std::vector<unsigned char> cpuData{ 12,200,15,16,17};
				XLOG_INFO(u8"[2] 纹理数据上传");
				tex->setSubData1D(0, num_gpu, cpuData.data());

				auto raw_data = tex->map2pbo()->map2cpu();
				auto s = raw_data->heteroCast<unsigned char>(1);
				XLOG_INFO(u8"[3] 回读纹理数据{} {}", "\n", s->toString());
			}
		}
	}
	context->doneCurrent();
}

void TestTexture1D_Array_R8_u8() {
	XLOG_DEBUG("TestTexture1D_Array_R8_u8");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();

	context->create(window->winId());
	if (context->makeCurrent()) {
		//为纹理分配数据
		auto cpuDataArray = makeShareDbObject<XUCharArray2D>();
		int width = 5;
		int height = 4;
		cpuDataArray->setDimensions(width, height);

		for (int r = 0; r < height; r++) {
			auto prow = cpuDataArray->data(r);
			for (int c = 0; c < width; c++) {
				prow[c] = r * width + c;
			}
		}

		XLOG_DEBUG(u8"[1] 一维数组纹理原始CPU数据{}{}", "\n", cpuDataArray->toString());

		XLOG_INFO(u8"[2] 纹理创建");
		auto tex = makeShareDbObject<XOpenGLTexture>();
		auto ss = tex->shared_from_this();
		tex->setTarget(XOpenGLTexture::Target1DArray);
		tex->create();
		tex->bind();
		tex->setInternalFormat(XOpenGLTexture::TextureFormat::R8U);
		tex->setExternalFormat(XOpenGLTexture::PixelFormat::Red_Integer, XOpenGLTexture::PixelType::UInt8);
		tex->texStorage2D(width, height);


		XLOG_INFO(u8"[3] 纹理数据上传");
		tex->setSubData2D(0, 0, width, height, cpuDataArray->data(0));

		//回读纹理数据，整个回读
		{
			auto raw_data = tex->map2pbo()->map2cpu();
			XLOG_INFO(u8"[4-1] 回读纹理字节数据{} {}", "\n", raw_data->toString());
		}

		//回读纹理数据，使用fbo+readpixel逐层回读
		{
			for (int i = 0; i < height; i++) {
				auto raw_data = tex->getSubData1D(0, width, 0, i);
				XLOG_INFO(u8"[4-2] 回读纹理字节数据 {}行:{}", i, raw_data->toString());
			}
		}

		//回读纹理数据
		{
			auto raw_data = tex->getSubData2D(0, 0, width, height);
			XLOG_INFO(u8"[4-2] 回读纹理字节数据 {}", raw_data->toString());
		}
	}
	context->doneCurrent();
	return;
}

void TestTexture2D() {
	
	XLOG_DEBUG("TestTexture");
	auto window = makeShareDbObject<XOffsetWindow>();
	auto context = makeShareDbObject<XOpenGLContext>();
	
	context->create(window->winId());
	json j = XOpenGLFuntion::xglGetExtensions();
	//std::cout << j.dump(4) << std::endl;
	if (context->makeCurrent()) {
		int width = 5;
		int height =4;
		auto texture = makeShareDbObject<XOpenGLTexture>();
		texture->setTarget(XOpenGLTexture::Target2D);
		texture->setInternalFormat(XOpenGLTexture::TextureFormat::R32U);
		texture->setExternalFormat(XOpenGLTexture::Red_Integer, XOpenGLTexture::UInt32);
		texture->create();
		texture->bind();
		texture->texStorage2D(width, height);
		//准备CPU端数据
		auto cpuData = makeShareDbObject<XUIntArray2D>();
		cpuData->setDimensions(width,height);
		for (int r = 0; r < height; r++) {
			auto pcol = cpuData->data(r);
			for (int c = 0; c < width; c++) {
				pcol[c] = r*width+c;
			}
		}

		XLOG_INFO(u8"[1] CPU端数据 {}{}","\n", cpuData->toString());
		
		XLOG_INFO(u8"[2] 纹理数据上传");
		texture->setSubData2D(0, 0, width, height, cpuData->data());

		XLOG_INFO(u8"[3] 纹理数据回读");
		auto row_data = texture->map2pbo()->map2cpu();
		auto ss =row_data->heteroCast<uint32_t>(1);
		XLOG_INFO(u8"[4-1] 回读纹理数据{} {}", "\n", ss->toString());
		auto gpuData = makeShareDbObject<XUIntArray2D>(width,height,1);
		gpuData->memCopy(ss);
		XLOG_INFO(u8"[4-2] 回读纹理数据{} {}", "\n", gpuData->toString());

		auto gpuData_2d =texture->getSubData2D(0,0,width,height);
		auto ss2 = gpuData_2d->heteroCast<uint32_t>(width,1);
		XLOG_INFO(u8"[4-3] 回读纹理数据{} {}", "\n", ss2->toString());

	}
	context->doneCurrent();
}

int main() {
	SetConsoleOutputCP(65001);
	XTestApp app("XOpenGL测试");

	app.addCmd("TestOffsetWindow", u8"离线渲染环境搭建测试",TestOffsetWindow);
	app.addCmd("CreateFbo", u8"创建FBO", CreateFbo);
	app.addCmd("TestTexture1D_R8U", u8"1D纹理的测试 R8U", TestTexture1D_R8U);
	app.addCmd("TestTexture1D_R8_float", u8"1D纹理的测试 R8 float", TestTexture1D_R8_float);
	app.addCmd("TestTexture1D_R8_u8", u8"1D纹理的测试 R8 u8", TestTexture1D_R8_u8);
	app.addCmd("TestTexture1D_R32U", u8"1D纹理的测试 R32U", TestTexture1D_R32U);
	app.addCmd("TestTexture1D_Depth", u8"1D纹理的测试 深度、模板纹理", TestTexture1D_Depth);
	app.addCmd("TestTexture1D_Array_R8_u8", u8"1D纹理的测试 Array_R8_u8", TestTexture1D_Array_R8_u8);

	app.addCmd("TestTexture2D", u8"2D纹理的测试", TestTexture2D);

	return app.run();
}