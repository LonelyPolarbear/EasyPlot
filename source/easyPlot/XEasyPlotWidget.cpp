#include "XEasyPlotWidget.h"

#include "render/XRender.h"
#include "render/XOpenGLRenderWindow.h"
#include "render/XRenderCamera.h"

#include "lib02_camera/xcamera.h"
#include <xalgo/XAlgo.h>

#include "xrendernode/XGeometryNode.h"
#include "xrendernode/renderNode3d/XArrowRenderNode.h"
#include "xrendernode/mapper/XPolyDataMapper.h"

#include "xrendernode/datasource/xchamferCubeSource.h"
#include "xrendernode/datasource/xconeSource.h"
#include "xrendernode/datasource/xcylinderSource.h"
#include "xrendernode/datasource/xregularPrimSource.h"
#include "xrendernode/datasource/xTorusSource.h"
#include "xrendernode/datasource/xfrustumSource.h"
#include "xrendernode/datasource/xcubeSource.h"
#include "xrendernode/filter/xshapeSourceTransformFilter.h"
#include "xrendernode/filter/xshapeSourceCombineFilter.h"
#include "xrendernode/filter/xshapeSourceCombineFilter.h"

#include <xrendernode/renderNode3d/XInfinitePlaneRenderNode.h>
#include <xrendernode/renderNode3d/XRectRenderNode.h>
#include <xrendernode/renderNode3d/XSphereRenderNode.h>
#include <xrendernode/renderNode3d/XTransformGizmoRenderNode.h>
#include <xrendernode/renderNode3d/XTorusRenderNode.h>
#include <xrendernode/renderNode3d/XTextRenderNode.h>

#include <lib04_opengl/XOpenGLBuffer.h>

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>


XEasyPlotWidget::XEasyPlotWidget(QWidget* parent) :XGLWidget(parent)
{
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
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
		cmaeraType.push_back(XRenderCamera::ProjectionType::ortho);

		std::vector< XRenderCamera::CameraStyle> cmaeraStyle;
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::upRight);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);
		cmaeraStyle.push_back(XRenderCamera::CameraStyle::freely);

		mRenderWindow->setName("renderWindow");

		std::vector<sptr<XRender>> renders;
		XQ::XAlgo::XGridLayout lay(2,2);
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				auto viewport = lay.getCellPos(x, y);
				auto render = makeShareDbObject<XRender>();
				render->setActive(false);
				render->setBackGroundColorTop(colors[y * 2 + x]);
				render->setViewPort(viewport[0], viewport[1], viewport[2], viewport[3]);
				render->getCamera()->setProjectionType(cmaeraType[y * 2 + x]);
				auto ss = render->getCamera()->AttrCameraStyle;
				render->getCamera()->AttrCameraStyle->setValue(cmaeraStyle[y * 2 + x]);
				
				mRenderWindow->addRender(render);
				renders.push_back(render);
				render->fitView();
			}
		}

		renders[0]->AddCubeTestNode();
		renders[0]->AddInfinitePlaneNode();
		renders[0]->AddAxisNode();

		renders[1]->AddCubeTestNode();
		renders[1]->AddInfinitePlaneNode();
		renders[1]->AddAxisNode();


		renders[2]->AddAxisNode();
		renders[3]->AddAxisNode();

		#if 1
		//创建一个视锥
		{
			//关联1-3的render
			sptr<XGeometryNode> frustumNode = makeShareDbObject<XGeometryNode>();
			sptr<xfrustumSource> frustum = makeShareDbObject<xfrustumSource>();
			frustum->setFarPlanePoints({ {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}, {-1, -1, 1} });
			frustum->setNearPlanePoints({ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}, {-1, -1, -1} });
			frustumNode->setInput(frustum);
			frustumNode->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
			frustumNode->setPolygonMode(PolygonMode::line_and_face);
			frustumNode->setColorMode(ColorMode::FaceColor);
			renders[3]->addRenderNode3D(frustumNode);

			sptr<XRectRenderNode> rectNode = makeShareDbObject<XRectRenderNode>();
			rectNode->setSingleColor(XQ::Vec4f(1, 0, 0, 0.2));
			rectNode->setColorMode(ColorMode::SingleColor);
			renders[3]->addRenderNode3D(rectNode);

			renders[1]->getCamera()->sigDataChanged.connect([frustum, r = renders[3], rectNode](sptr<XDataObject> camera, XDataChangeType type) {
				if (type == XDataChangeType::DataModified) {
					auto c = camera->asDerived<XRenderCamera>();
					if (c) {
						auto ss = c->getFrustumInWorld();
						frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
						frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });

						auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
						rectNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
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

				sptr<XRectRenderNode> rectNode = makeShareDbObject<XRectRenderNode>();
				rectNode->setSingleColor(XQ::Vec4f(1, 0, 0, 0.2));
				rectNode->setColorMode(ColorMode::SingleColor);
				renders[2]->addRenderNode3D(rectNode);

				

				renders[0]->getCamera()->sigDataChanged.connect([frustum, r = renders[2], rectNode](sptr<XDataObject> camera, XDataChangeType type) {
					if (type == XDataChangeType::DataModified) {
						auto c = camera->asDerived<XRenderCamera>();
						if (c) {
							auto ss = c->getFrustumInWorld();
							frustum->setNearPlanePoints({ ss[0],ss[1],ss[2],ss[3] });
							frustum->setFarPlanePoints({ ss[4],ss[5],ss[6],ss[7] });
							auto intersections = XQ::XAlgo::getFrustumXOZIntersections(ss.data(), ss.data() + 4);
							rectNode->setRect({ intersections[0],intersections[1] ,intersections[2] ,intersections[3] });
							
						}
					}
					});
			}
#endif
		}
		#endif
	}
}

void XEasyPlotWidget::test2()
{
	{
		auto render = makeShareDbObject<XRender>();
		render->getCamera()->setProjectionType(XBaseRenderCamera::ProjectionType::ortho);
		render->setViewPort(0,0,0.5,1);
		render->setName("renderLeft");
		mRenderWindow->addRender(render);
		mRenderWindow->setName("renderWindow");
		render->getCamera()->AttrCameraStyle->setValue(XRenderCamera::CameraStyle::freely);
		render->AddAxisNode();
		render->AddCubeTestNode();
		render->AddInfinitePlaneNode();
	}

	{
		auto render = makeShareDbObject<XRender>();
		render->getCamera()->setProjectionType(XBaseRenderCamera::ProjectionType::perspective);
		render->setBackGroundColorTop(0xFF, 0xF1, 0xEB, 255);
		render->setBackGroundColorBot(0xAC, 0xE0, 0xF9, 255);
		render->setViewPort(0.5, 0, 0.5, 1);
		render->setName("renderRight");
		mRenderWindow->addRender(render);
		render->getCamera()->AttrCameraStyle->setValue(XRenderCamera::CameraStyle::freely);
		render->AddAxisNode();
		render->AddCubeTestNode();
		render->AddInfinitePlaneNode();
	}
	
	//HighFive::File file("88888test.h5", HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
	//coneNode->serialize(file.createGroup(coneNode->getName()));
}

void XEasyPlotWidget::test3()
{
	auto render = makeShareDbObject<XRender>();
	mRenderWindow->addRender(render);
	
	#if 0
	{
		sptr<XConeSource> coneSource = makeShareDbObject<XConeSource>();
		coneSource->Modified();
		coneSource->setNumVertices(8);
		coneSource->setAngle(240);
		sptr<XShapeSourceTransformFilter> transformFilter = makeShareDbObject<XShapeSourceTransformFilter>();
		transformFilter->setInput(coneSource);
		sptr<XGeometryNode> coneNode = makeShareDbObject<XGeometryNode>();


		sptr<XCylinderSource> primSource = makeShareDbObject<XCylinderSource>();
		primSource->Modified();

		sptr<XShapeSourceTransformFilter> transformFilter2 = makeShareDbObject<XShapeSourceTransformFilter>();
		transformFilter2->setInput(primSource);
		transformFilter2->translate(2, 0, 0);

		sptr<XShapeSourceCombineFilter> combineSource = makeShareDbObject<XShapeSourceCombineFilter>();
		combineSource->addInput(transformFilter);
		combineSource->addInput(transformFilter2);

		sptr<XGeometryNode> Node = makeShareDbObject<XGeometryNode>();
		Node->setInput(combineSource);
		Node->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
		Node->setPolygonMode(PolygonMode::all);
		Node->setColorMode(ColorMode::FaceColor);
		render->addRenderNode3D(Node);
	}
	#endif

	sptr<XArrowRenderNode> Node = makeShareDbObject<XArrowRenderNode>();
	Node->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
	Node->setPolygonMode(PolygonMode::all);
	Node->setColorMode(ColorMode::FaceColor);
	render->addRenderNode3D(Node);

	Node->setLineSize(2, 20);
	Node->setArrowSize(3,5);

	{
		sptr<XArrowRenderNode> Node = makeShareDbObject<XArrowRenderNode>();
		Node->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
		Node->setPolygonMode(PolygonMode::all);
		Node->setColorMode(ColorMode::FaceColor);
		render->addRenderNode3D(Node);

		Node->setLineSize(2, 20);
		Node->setArrowSize(3, 5);

		Node->rotateY(90);
	}

	render->getCamera()->AttrCameraStyle->setValue(XRenderCamera::CameraStyle::freely);
}

void XEasyPlotWidget::test4()
{
	auto render = makeShareDbObject<XRender>();
	mRenderWindow->addRender(render);

	render->getCamera()->AttrCameraStyle->setValue(XRenderCamera::CameraStyle::freely);
	render->AddAxisNode();

	auto text = makeShareDbObject<XTextRenderNode>();
	text->Attribute->AttrSizePolicy->AttrIsFixedOrien->setValue(true);
	text->Attribute->AttrSizePolicy->AttrIsFixedSize->setValue(true);
	text->Attribute->AttrSizePolicy->AttrPositionType->setValue(XRenderNodeOriginPositionType::fixed);
	text->Attribute->AttrSizePolicy->AttrPositionPos->setValue(XQ::Vec2i(100,100));
	text->Attribute->AttrSizePolicy->AttrFixedPixel->setValue(XQ::Vec3i(200, 64, 1));
	text->Attribute->AttrUseNormalCamera->setValue(false);
	text->setText(L"XQQ");
	render->addScreenRenderNode3D(text);
}

//圆环测试
void XEasyPlotWidget::test5()
{
	auto render = makeShareDbObject<XRender>();
	mRenderWindow->addRender(render);
	sptr<XTorusRenderNode> Node = makeShareDbObject<XTorusRenderNode>();

	Node->setColorMode(ColorMode::FaceColor);
	Node->setSingleColor(XQ::Vec4f(1,0,0,1));
	Node->setPolygonMode(PolygonMode::face);

	render->addRenderNode3D(Node);
	render->getCamera()->AttrCameraStyle->setValue(XRenderCamera::CameraStyle::freely);
}

void XEasyPlotWidget::slotAddCube()
{
	auto renders = getRenderWindow()->getRenders();
	for (auto r : renders) {
		if (r->isActive()) {
			sptr<XGeometryNode> cubeNode = makeShareDbObject<XGeometryNode>();
			cubeNode->setName("cube");
			sptr<XCubeSource> cubesource = makeShareDbObject<XCubeSource>();
			cubeNode->setInput(cubesource);
			cubeNode->setSingleColor(XQ::Vec4f(0, 0, 0, 1));
			cubeNode->setSelectedColor(XQ::Vec4f(1, 0, 0, 1));
			cubeNode->setPolygonMode(PolygonMode::all);
			cubeNode->setColorMode(ColorMode::FaceColor);
			cubeNode->scale(10, 10, 10);
			cubeNode->translate(0, 1, 0);
			cubeNode->State->AttrHasSelect->setValue(true);
			cubesource->Modified();
			r-> addRenderNode3D(cubeNode);
		}
	}
}

void XEasyPlotWidget::slotFitView3D()
{
	for (auto r : mRenderWindow->getRenders()) {
		if (r->isActive())
			r->fitView();
	}
}