#include <lib04_opengl/XOpenGLContext.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>

#include <xtest/XTest.h>
#include <xlog/XLogger.h>

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

int main() {
	XTestApp app("XOpenGL测试");

	app.addCmd("TestOffsetWindow", "离线渲染环境搭建测试",TestOffsetWindow);
	app.addCmd("CreateFbo", "创建FBO", CreateFbo);

	return app.run();
}