#include "XEasyPlotWidget.h"
#include "lib05_shape/datasource/xchamferCubeSource.h"
#include "lib05_shape/datasource/xconeSource.h"
#include "render/XRender.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderCamera.h"
#include "lib05_shape/XGeometryNode.h"
#include "lib05_shape/mapper/XPolyDataMapper.h"
#include "lib02_camera/xcamera.h"
#include <xalgo/XAlgo.h>


XEasyPlotWidget::XEasyPlotWidget(QWidget* parent) :XGLWidget(parent)
{
	{
		//2*2²¼¾Ö
		std::vector<XQ::Vec4u8> colors;
		colors.push_back(XQ::Vec4u8(176, 192, 222, 255));
		colors.push_back(XQ::Vec4u8(119, 136, 153, 255));
		colors.push_back(XQ::Vec4u8(112, 128, 144, 255));
		colors.push_back(XQ::Vec4u8(245, 245, 220,255));

		std::vector< XRenderCamera::ProjectionType> cmaeraType;
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::perspective);
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::perspective);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XGeometryNode> cubeActor = makeShareDbObject<XGeometryNode>();

		cubeSource->Modified();

		cubeActor->setInput(cubeSource);

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::fill);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		XQ::XAlgo::XGridLayout lay(2,2);
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				auto viewport = lay.getCellPos(x,y);
				auto render = makeShareDbObject<XRender>();
				render->setActive(false);
				render->setBackGroundColor1(colors[y*2+x]);
				render->setViewPort(viewport[0], viewport[1], viewport[2], viewport[3]);
				render->getCamera()->setProjectionType(cmaeraType[y * 2 + x]);
				mRenderWindow->addRender(render);
				render->addRenderNode3D(cubeActor);
				render->fitView();
			}
		}
	}
	#if 0
	{
		auto render1 = makeShareDbObject<XRender>();
		render1->setActive(false);
		render1->setBackGroundColor1(176, 192, 222, 255);
		render1->setViewPort(0, 0, 0.5, 1);
		mRenderWindow->addRender(render1);

		auto render2 = makeShareDbObject<XRender>();
		render2->setActive(false);
		render2->setBackGroundColor1(119, 136, 153, 255);
		render2->setViewPort(0.5, 0, 0.5, 1);
		render2->getCamera()->getInnerCamera()->setType(xcamera::cameraType::perspective);
		mRenderWindow->addRender(render2);

		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XGeometryNode> cubeActor = makeShareDbObject<XGeometryNode>();

		cubeSource->Modified();

		cubeActor->setInput(cubeSource);

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::fill);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		render1->addRenderNode3D(cubeActor);

		render2->addRenderNode3D(cubeActor);

		render1->fitView();

		render2->fitView();
	}
	#endif
}

XEasyPlotWidget::~XEasyPlotWidget()
{
}