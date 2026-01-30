#include "XEasyPlotWidget.h"
#include "lib05_shape/datasource/xchamferCubeSource.h"
#include "lib05_shape/datasource/xconeSource.h"
#include "lib05_shape/datasource/xfrustumSource.h"
#include "render/XRender.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderCamera.h"
#include "lib05_shape/XGeometryNode.h"
#include "lib05_shape/mapper/XPolyDataMapper.h"
#include "lib02_camera/xcamera.h"
#include <xalgo/XAlgo.h>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <lib05_shape/renderNode3d/XInfinitePlaneRenderNode.h>
#include <lib05_shape/renderNode3d/XRectRenderNode.h>
#include <lib04_opengl/XOpenGLBuffer.h>


XEasyPlotWidget::XEasyPlotWidget(QWidget* parent) :XGLWidget(parent)
{
	test1();
	//test2();
}

XEasyPlotWidget::~XEasyPlotWidget()
{
}

void XEasyPlotWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu* menu = new QMenu(this);

	QAction* ActFitView = menu->addAction(QIcon(":/icon/fitView.svg"), "FitView3D");

	connect(ActFitView, &QAction::triggered, this, &XEasyPlotWidget::slotFitView3D);

	menu->show();
	menu->exec(event->globalPos()); // event->pos()是相对于当前控件的位置
}

void XEasyPlotWidget::test1()
{
	{
		//2*2布局
		std::vector<XQ::Vec4u8> colors;
		colors.push_back(XQ::Vec4u8(176, 192, 222, 255));
		colors.push_back(XQ::Vec4u8(119, 136, 153, 255));
		colors.push_back(XQ::Vec4u8(112, 128, 144, 255));
		colors.push_back(XQ::Vec4u8(245, 245, 220, 255));

		std::vector< XRenderCamera::ProjectionType> cmaeraType;
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::perspective);
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::perspective);

		std::vector< XRenderCamera::CameraStyle> cmaeraStyle;
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::upRight);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);

#if 1
		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XGeometryNode> cubeActor = makeShareDbObject<XGeometryNode>();

		cubeSource->Modified();

		cubeActor->setInput(cubeSource);

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::face);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));

		cubeActor->scale(100, 100, 100);

		cubeActor->translate(0, 1, 0);
#endif

		std::vector<sptr<XRender>> renders;
		XQ::XAlgo::XGridLayout lay(2, 2);
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				auto viewport = lay.getCellPos(x, y);
				auto render = makeShareDbObject<XRender>();
				render->setActive(false);
				render->setBackGroundColor1(colors[y * 2 + x]);
				render->setViewPort(viewport[0], viewport[1], viewport[2], viewport[3]);
				render->getCamera()->setProjectionType(cmaeraType[y * 2 + x]);
				auto ss = render->getCamera()->AttrCameraStyle;
				render->getCamera()->AttrCameraStyle->setValue((uint32_t)cmaeraStyle[y * 2 + x]);
				mRenderWindow->addRender(render);
				renders.push_back(render);
				render->fitView();
			}
		}
		renders[0]->addRenderNode3D(cubeActor);
		renders[2]->addRenderNode3D(cubeActor);
		renders[1]->addRenderNode3D(cubeActor);
		renders[3]->addRenderNode3D(cubeActor);

		//创建一个视锥
		{
			//关联1-3的render
			sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
			sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
			frustum->setFarPlanePoints({ {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}, {-1, -1, 1} });
			frustum->setNearPlanePoints({ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1} });
			frustumNode->setInput(frustum);
			frustumNode->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
			frustumNode->setPolygonMode(PolygonMode::line);
			frustumNode->setColorMode(ColorMode::SingleColor);
			renders[3]->addRenderNode3D(frustumNode);

			sptr<XRectRenderNode> rectNode = makeShareDbObject<XRectRenderNode>();
			rectNode->setSingleColor(XQ::Vec4f(1, 0, 0, 0.2));
			rectNode->setColorMode(ColorMode::SingleColor);
			renders[3]->addRenderNode3D(rectNode);
			//renders[3]添加无限平面
			{
				/*sptr<XInfinitePlaneRenderNode> InfinitePlaneNode = makeShareDbObject<XInfinitePlaneRenderNode>();
				renders[3]->addRenderNode3D(InfinitePlaneNode);
				renders[3]->getCamera()->sigDataChanged.connect([InfinitePlaneNode](sptr<XDataObject> camera, XDataChangeType type) {
					if (type == XDataChangeType::DataModified) {
						auto c = camera->asDerived<XRenderCamera>();
						if (c) {
							auto ss = c->getFrustumInWorld();
							auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
							InfinitePlaneNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
						}
					}
					});*/
			}

			sptr<XInfinitePlaneRenderNode> InfinitePlaneNode = makeShareDbObject<XInfinitePlaneRenderNode>();
			renders[1]->addRenderNode3D(InfinitePlaneNode);

			renders[1]->getCamera()->sigDataChanged.connect([frustum, r = renders[3], rectNode, InfinitePlaneNode](sptr<XDataObject> camera, XDataChangeType type) {
				if (type == XDataChangeType::DataModified) {
					auto c = camera->asDerived<XRenderCamera>();
					if (c) {
						auto ss = c->getFrustumInWorld();
						frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
						frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });

						auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
						rectNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });

						InfinitePlaneNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
					}
				}
				});
		}
		{
#if 1
			{
				//关联0-2的render
				sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
				sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
				frustum->setFarPlanePoints({ {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}, {-1, -1, 1} });
				frustum->setNearPlanePoints({ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1} });
				frustumNode->setInput(frustum);
				frustumNode->setSingleColor(XQ::Vec4f(0, 1, 0, 0.3));
				frustumNode->setPolygonMode(PolygonMode::line);
				frustumNode->setColorMode(ColorMode::SingleColor);
				renders[2]->addRenderNode3D(frustumNode);
				sptr<XInfinitePlaneRenderNode> InfinitePlaneNode = makeShareDbObject<XInfinitePlaneRenderNode>();
				renders[0]->addRenderNode3D(InfinitePlaneNode);

				renders[0]->getCamera()->sigDataChanged.connect([frustum, r = renders[2], InfinitePlaneNode](sptr<XDataObject> camera, XDataChangeType type) {
					if (type == XDataChangeType::DataModified) {
						auto c = camera->asDerived<XRenderCamera>();
						if (c) {
							auto ss = c->getFrustumInWorld();
							frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
							frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });
							auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
							InfinitePlaneNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
						}
					}
					});
			}
#endif
		}
	}
}

void XEasyPlotWidget::test2()
{
	auto render = makeShareDbObject<XRender>();
	mRenderWindow->addRender(render);
	sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
	sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
	frustum->setFarPlanePoints({ {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1} });
	frustum->setNearPlanePoints({ {-1, 1, -1}, {-1, -1, -1}, {1, -1, -1}, {1, 1, -1} });
	frustumNode->setInput(frustum);
	frustumNode->setSingleColor(XQ::Vec4f(1, 1, 1, 1));
	frustumNode->setPolygonMode(PolygonMode::line_and_face);
	frustumNode->setColorMode(ColorMode::FaceColor);
	render->addRenderNode3D(frustumNode);
	render->getCamera()->AttrCameraStyle->setValue((uint32_t)XRenderCamera::CameraStyle::freely);
}

void XEasyPlotWidget::slotFitView3D()
{
	for (auto r : mRenderWindow->getRenders()) {
		if (r->isActive())
			r->fitView();
	}
}