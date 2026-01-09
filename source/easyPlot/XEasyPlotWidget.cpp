#include "XEasyPlotWidget.h"
#include "lib05_shape/datasource/xchamferCubeSource.h"
#include "lib05_shape/datasource/xconeSource.h"
#include "render/XRender.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderCamera.h"
#include "lib05_shape/xshape.h"
#include "lib02_camera/xcamera.h"


XEasyPlotWidget::XEasyPlotWidget(QWidget* parent):XGLWidget(parent)
{
#if 1
	{
		auto render = makeShareDbObject<XRender>();
		render->setActive(false);
		render->setBackGroundColor1(255, 0, 0, 255);
		render->setViewPort(0, 0, 0.5, 1);
		mRenderWindow->addRender(render);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XShape> cubeActor = makeShareDbObject<XShape>();

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::fill);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		cubeActor->setInput(cubeSource);

		cubeSource->Modified();

		render->addActor3D(cubeActor);

		render->fitView();
	}
#endif
#if 1
	{
		auto render2 = makeShareDbObject<XRender>();
		render2->setActive(false);
		render2->setBackGroundColor1(0, 255, 0, 255);
		render2->setViewPort(0.5, 0, 0.5, 1);
		render2->getCamera()->getInnerCamera()->setType(xcamera::cameraType::perspective);
		mRenderWindow->addRender(render2);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XShape> cubeActor = makeShareDbObject<XShape>();

		cubeActor->setColorMode(ColorMode::SingleColor);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		cubeActor->setInput(cubeSource);

		cubeSource->Modified();

		render2->addActor3D(cubeActor);

		render2->fitView();
	}
#endif
}

XEasyPlotWidget::~XEasyPlotWidget()
{
}
