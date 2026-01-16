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


XEasyPlotWidget::XEasyPlotWidget(QWidget* parent) :XGLWidget(parent)
{
	{
		//2*2布局
		std::vector<XQ::Vec4u8> colors;
		colors.push_back(XQ::Vec4u8(176, 192, 222, 255));
		colors.push_back(XQ::Vec4u8(119, 136, 153, 255));
		colors.push_back(XQ::Vec4u8(112, 128, 144, 255));
		colors.push_back(XQ::Vec4u8(245, 245, 220,255));

		std::vector< XRenderCamera::ProjectionType> cmaeraType;
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::perspective);
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);

		#if 1
		sptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();

		sptr<XGeometryNode> cubeActor = makeShareDbObject<XGeometryNode>();

		cubeSource->Modified();

		cubeActor->setInput(cubeSource);

		cubeActor->setColorMode(ColorMode::FaceColor);

		cubeActor->setPolygonMode(PolygonMode::fill);

		cubeActor->setSingleColor(XQ::Vec4f(0, 0, 1, 1));
		#endif

		std::vector<sptr<XRender>> renders;
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
				renders.push_back(render);
				render->fitView();
			}
		}

		//创建一个视锥
		{
			//关联1-3的render
			sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
			sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
			frustum->setFarPlanePoints({ {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}, {-1, -1, 1} });
			frustum->setNearPlanePoints({ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1} });
			frustumNode->setInput(frustum);
			frustumNode->setSingleColor(XQ::Vec4f(0, 0, 1, 0.3));
			renders[3]->addRenderNode3D(frustumNode);

			renders[1]->getCamera()->sigDataChanged.connect([frustum, r = renders[3]](sptr<XDataObject> camera, XDataChangeType type) {
				if (type == XDataChangeType::DataModified) {
					auto c = camera->asDerived<XRenderCamera>();
					if (c) {
						auto ss = c->getFrustumInWorld();
						frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
						frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });
					}
				}
				});
		}
		{
			{
				//关联1-3的render
				sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
				sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
				frustum->setFarPlanePoints({ {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}, {-1, -1, 1} });
				frustum->setNearPlanePoints({ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1} });
				frustumNode->setInput(frustum);
				frustumNode->setSingleColor(XQ::Vec4f(0, 1, 0, 0.3));
				renders[2]->addRenderNode3D(frustumNode);
				renders[0]->getCamera()->sigDataChanged.connect([frustum, r = renders[2]](sptr<XDataObject> camera, XDataChangeType type) {
					if (type == XDataChangeType::DataModified) {
						auto c = camera->asDerived<XRenderCamera>();
						if (c) {
							auto ss = c->getFrustumInWorld();
							frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
							frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });
						}
					}
					});
			}
		}
	}
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
	//d->mouseMoveRealTimePos = event->globalPos();
	menu->exec(event->globalPos()); // event->pos()是相对于当前控件的位置
}

void XEasyPlotWidget::slotFitView3D()
{
	for (auto r : mRenderWindow->getRenders()) {
		if(r->isActive())
			r->fitView();
	}
}
