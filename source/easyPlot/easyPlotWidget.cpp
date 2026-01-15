
//#pragma execution_character_set("utf-8")

#include "easyPlotWidget.h"
#include <glew/glew.h>
#include <windows.h>
#include <Eigen/Eigen>
#include <lib01_shader/xshader.h>
#include <lib01_shader/XComputeShader.h>
#include <lib01_shader/xshaderManger.h>
#include <lib02_camera/xcamera.h>
#include <lib00_utilty/XUtilty.h>

#include <QMouseEvent> 
#include <QWheelEvent> 
#include <iostream>
#include <QDebug>
#include <lib03_stbImage/stbImage.h>

#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLTexture.h>
#include <lib04_opengl/XOpenGLContext.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib04_opengl/XOpenGLFramebufferObject.h>

#include <lib05_shape/datasource/xsphereSource.h>
#include <lib05_shape/datasource/xfileSource.h>
#include <lib05_shape/datasource/xcubeSource.h>
#include <lib05_shape/datasource/xchamferCubeSource.h>
#include <lib05_shape/datasource/xregularPrimSource.h>
#include <lib05_shape/datasource/xcylinderSource.h>
#include <lib05_shape/datasource/xconeSource.h>
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/XGraphicsItem.h>
#include <lib05_shape/actor2d/XRectItem.h>
#include <lib05_shape/actor2d/XLineItem.h>
#include <lib05_shape/actor2d/XBarItem.h>
#include <lib05_shape/actor2d/XChartItem.h>
#include <lib05_shape/actor2d/XChartItem2.h>
#include <lib05_shape/actor2d/XPolyline.h>
#include <lib05_shape/actor2d/XTextItem.h>
#include <lib05_shape/actor2d/XScreenTextItem.h>
#include <lib05_shape/actor2d/XAxisItem.h>
#include <lib05_shape/actor2d/XGridItem.h>
#include <lib05_shape/actor2d/XCheckBoxItem.h>
#include <lib05_shape/actor2d/XLegendRowItem.h>
#include <lib05_shape/actor2d/XLegendItem.h>

#include <lib06_select/xviewselection.h>
#include <lib07_scene/xscene.h>

#include <QTimer>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QFutureWatcher>
#include <QtConcurrent> 

#include <lib08_freetype/xfreetype.h>
#include <qapplication.h>
#include <QFileDialog>
#include <lib09_panel/FontSetDlg.h>
#include <lib09_panel/ScreenShotDlg.h>
#include <lib09_panel/FboTest.h>
#include <lib09_panel/GridSetDlg.h>

#include <lib04_opengl/XOpenGLFuntion.h>
#include <QMessageBox>

using namespace std::chrono_literals;
enum class CameraAction {
	None = 1,
	Rotate = 1 << 1,
	Move = 1 << 2
};

struct easyPlotWidget::Internal {
	std::shared_ptr<xShaderManger> shaderManger = makeShareDbObject<xShaderManger>();
	std::shared_ptr<XScene> scene = makeShareDbObject<XScene>();
	std::shared_ptr<XGraphicsItem> rect = makeShareDbObject<XRectItem>(nullptr);		//实时绘制的矩形
	std::shared_ptr<XTextItem> screenTextItem = makeShareDbObject<XTextItem>(nullptr);		//实时绘制的矩形
	std::shared_ptr<XTextItem> screenTextItemLine = makeShareDbObject<XTextItem>(nullptr);		//实时绘制的矩形

	std::shared_ptr<XTextItem> screenTextItemMousePos = makeShareDbObject<XTextItem>(nullptr);		//实时显示鼠标的当前位置
	
	//鼠标的位置实时更新
	QPoint mouseMoveLastTimePos = QPoint(0, 0);							//鼠标实时移动第一个位置
	QPoint mouseMoveRealTimePos = QPoint(0, 0);							//鼠标实时移动第二个位置
	QPoint mousePressPos = QPoint(0, 0);											//鼠标左键按下位置

	std::set<uint32_t> select3dObject;			//选中的三维物体
	std::set<uint32_t> select2dObject;		//选中的二维物体

	CameraAction cameraAction = CameraAction::None;
	QTimer *timer = nullptr;

	uint32_t mSelectId = 0;		//当前鼠标中键选中的Id
	QFutureWatcher<std::shared_ptr< XFileSource>>* mFutureWatcher;

	QFutureWatcher<void>* mFutureWatcherVoid;

	bool rectPickEnable = true;
	bool mIsMouseLeftPress = false;
	bool mIsMouseRightPressMoved = false;
	bool mIsMouseRightPress = false;

	graphicsItemType mDrawItemType = graphicsItemType::none;
	int mDrawItemMethod = -1;

	//当前绘制的数据和类型
	DarwItemData mDrawItemData;
};

easyPlotWidget::easyPlotWidget(QWidget* parent) :XOpenGLWidget(parent),d(std::make_unique<Internal>())
{
	qRegisterMetaType<std::shared_ptr<XFileSource>>("std::shared_ptr<XFileSource>");
	d->mFutureWatcher = new QFutureWatcher<std::shared_ptr< XFileSource>>();
	d->mFutureWatcherVoid = new QFutureWatcher<void>();

	setFocusPolicy(Qt::StrongFocus); // 添加这行
	setMouseTracking(true);
	//d->timer = new QTimer(this);
	//d->timer->setInterval(100);
	//connect(d->timer, &QTimer::timeout, this, &easyPlotWidget::timerOut);

	//d->timer->start();

	buildUI();
	attachSignalSlot();

	if (!QResource::registerResource(qApp->applicationDirPath() + "/easyPlot.rcc")) {
	   std::cout << "register resource failed" << std::endl;
	}


	QFuture<void> future = QtConcurrent::run([]()->void {
		auto ss = XQ::ShareVar::instance().currentExeDir + "\\sdf\\data.txt";
		xfreetype::Instance()->LoadGlyphSdf(QString::fromStdString(ss));
		});


	d->mFutureWatcherVoid->setFuture(future);
}

easyPlotWidget::~easyPlotWidget()
{
}

void easyPlotWidget::render()
{
#if 1
	d->scene->render();
	swapBuffers();
#else
	makeCurrent();
	glViewport(0, 0, 300, 300);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT); 

	glViewport(300, 0, 300, 300);

	glGetIntegerv(GL_VIEWPORT, vp);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);  

	swapBuffers();
	doneCurrent();
#endif
}

void easyPlotWidget::initGLResource()
{
	//创建opengl资源
	d->shaderManger->initGLResource();		//创建所有需要的shader

	auto cameraLeft = makeShareDbObject<xcamera>();

	d->scene->setCamera(cameraLeft);
	d->scene->setShaderManger(d->shaderManger);
	d->scene->setContext(getContext());

	{
		auto rect = d->rect;
		rect->setLineWidth(1);
		rect->setFixedLine(true);
		rect->setSingleColor(XQ::Vec4f(1, 1,1, 1));
		//rect->initiallize();
		rect->setVisible(false);
		d->scene->addGraphicsItem(rect);
	}

	{
		auto item = d->screenTextItem;
		//item->initiallize();
		item->setVisible(false);
		item->setPosition(250, 0);
		item->setVAlignment(XTextItem::VAlign::Top);
		item->setHAlignment(XTextItem::HAlign::Center);
		item->setPositionType(XGL::PositionType::sceneScreen_complete);
		item->setOrientation(XGL::Orientation::left_top);
		item->setFontSize(28);
		item->setIsFixWidth(true);
		item->setFixedWidth(500);
		item->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
		d->scene->addGraphicsItem(item);

		{
			auto item = d->screenTextItemLine;
			//item->initiallize();
			item->setVisible(false);
			item->setFontSize(28);
			item->setIsFixWidth(false);
			item->setVAlignment(XTextItem::VAlign::Top);
			item->setHAlignment(XTextItem::HAlign::Left);
			item->setPositionType(XGL::PositionType::sceneScreen_complete);
			item->setOrientation(XGL::Orientation::left_top);
			item->setPosition(10,0);
			item->setSingleColor(XQ::Vec4f(0, 1, 0, 1));
			d->scene->addGraphicsItem(item);
		}
	}

	//鼠标位置显示
	{
		auto item = d->screenTextItemMousePos;
		//item->initiallize();
		item->setVisible(true);
		item->setFontSize(20);
		item->setIsFixWidth(false);
		item->setVAlignment(XTextItem::VAlign::Top);
		item->setHAlignment(XTextItem::HAlign::Right);
		item->setPositionType(XGL::PositionType::sceneScreen_complete);
		item->setOrientation(XGL::Orientation::right_top);
		item->setPosition(0, 0);
		item->setSingleColor(XQ::Vec4f(0, 1, 0, 1));
		d->scene->addGraphicsItem(item);
	}

	print();
}

void easyPlotWidget::print()
{
	makeCurrent();

	std::cout << "全部纹理单元数量:" <<std::dec<< XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "顶点着色器纹理单元数量:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "片段着色器纹理单元数量:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "几何着色器纹理单元数量:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "曲面细分控制着色器最大纹理单元:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "曲面细分计算着色器最大纹理单元:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS) << std::endl;
	std::cout << "计算着色器最大纹理单元:" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS) << std::endl;


	std::cout << "顶点着色器支持的uniform组件数量：" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_VERTEX_UNIFORM_COMPONENTS) << std::endl;
	std::cout << "片段着色器支持的uniform组件数量：" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_FRAGMENT_UNIFORM_COMPONENTS) << std::endl;
	std::cout << "几何着色器支持的uniform组件数量：" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_GEOMETRY_UNIFORM_COMPONENTS) << std::endl;


	std::cout << "顶点着色器中uniform块的最大数量：" << std::dec << XOpenGLFuntion::xglGetMaxShaderComponentTypeNum(XOpenGL::ShaderComponentType::XGL_MAX_VERTEX_UNIFORM_BLOCKS) << std::endl;

	doneCurrent();
}

void easyPlotWidget::buildUI()
{
}

void easyPlotWidget::attachSignalSlot()
{
	connect(d->mFutureWatcher, &QFutureWatcher<std::shared_ptr<XFileSource>>::finished, this, &easyPlotWidget::slotFileLoadFinished);

	connect(d->mFutureWatcherVoid, &QFutureWatcher<void>::finished, this, &easyPlotWidget::slotAnyTaskFinished);
}

uint32_t easyPlotWidget::getSelectId()
{
	return d->mSelectId;
}

bool easyPlotWidget::isAllowContextmenu() const
{
	return !d->mIsMouseRightPressMoved;
}

void easyPlotWidget::mousePressEvent(QMouseEvent* event)
{
	//std::cout<<std::dec<< "鼠标摁下: (" << event->pos().x() << "," << event->pos().y() << ")" << std::endl;
	//左键旋转 右键平移  中键缩放
	d->mousePressPos = event->pos();
	if (event->button() == Qt::LeftButton) {
		d->mouseMoveLastTimePos = event->pos();

		d->select2dObject.clear();
		d->select3dObject.clear();
		//鼠标按下：记录当前选中的物体

		{
			auto selects = d->scene->getPointSelection(event->pos().x(), mHeight - event->pos().y());
			for (auto select : selects) {
				if (select.objectId != 0) {
					d->select3dObject.insert(select.objectId);
					std::cout << std::dec << "select object id:" << select.objectId << " primitive id:" << select.primitiveId << std::endl;
					d->mSelectId = select.objectId;
				}
			}
		}
		{
			auto selects = d->scene->getPointSelection2D(event->pos().x(), mHeight - event->pos().y());
			for (auto select : selects) {
				if (select.objectId != 0) {
					d->select2dObject.insert(select.objectId);
					std::cout << std::dec << "2d select object id:" << select.objectId << " primitive id:" << select.primitiveId << std::endl;
					d->mSelectId = select.objectId;
				}
			}
		}

		d->mIsMouseLeftPress = true;

		//如果启用了绘制，则左键按下时不启用相机交互
		if(d->mDrawItemType ==graphicsItemType::none)
			d->cameraAction = CameraAction::Rotate;	
		else {
			//创建一个Item
			d->mDrawItemData = createItem(d->mDrawItemType);
			if(d->mDrawItemData.item)
				d->scene->addGraphicsItem(d->mDrawItemData.item);
		}
	}
	else if (event->button() == Qt::MiddleButton) {
		d->cameraAction = CameraAction::Move;
		d->mouseMoveLastTimePos = event->pos();
	}

	if(event->button() == Qt::RightButton){
		d->mIsMouseRightPressMoved = false;
		d->mIsMouseRightPress = true;
	}

	return XOpenGLWidget::mousePressEvent(event);
}

void easyPlotWidget::mouseMoveEvent(QMouseEvent* event)
{	
	d->mouseMoveRealTimePos = event->pos();
	updateItem();

	if (d->mIsMouseRightPress) {
		d->mIsMouseRightPressMoved = true;
		if (!d->scene->getGraphicsItem(d->rect->getID())) {
			d->scene->addGraphicsItem(d->rect);
		}
		d->rect->resetTransform();
		d->rect->setVisible(true);

		auto s1 = d->scene->screenPos2ScenePos(mapToGLScreen(event->pos()));
		auto s2 = d->scene->screenPos2ScenePos(mapToGLScreen(d->mousePressPos));
		auto rect = std::dynamic_pointer_cast<XRectItem>(d->rect);
		if (rect) {
			rect->setRect(s1,s2);
		}
	}

	if ((uint32_t)d->cameraAction & (uint32_t)CameraAction::Rotate ) {

		d->scene->rotate(mapToGLScreen(d->mouseMoveRealTimePos),mapToGLScreen(d->mouseMoveLastTimePos));
		d->mouseMoveLastTimePos = d->mouseMoveRealTimePos;
	}
	else if ((uint32_t)d->cameraAction & (uint32_t)CameraAction::Move) {
		bool flag = true;
		{
			auto selectId = d->select2dObject.begin();
			if (selectId != d->select2dObject.end()) {
				auto selectIem = d->scene->getGraphicsItem(*selectId);
				if (auto chart = std::dynamic_pointer_cast<XChartItem2>(selectIem)) {
					auto s1 = d->scene->screenPos2ScenePos(mapToGLScreen(event->pos()));
					auto s2 = d->scene->screenPos2ScenePos(mapToGLScreen(d->mouseMoveLastTimePos));
					
					chart->chartTranslate(s2,s1);
					//chart->setPosition(s1.x, s1.y);
					flag = false;
				}
			}
		}
		if (flag) {
			d->scene->translate(mapToGLScreen(d->mouseMoveRealTimePos), mapToGLScreen(d->mouseMoveLastTimePos));
		}
		
		d->mouseMoveLastTimePos = d->mouseMoveRealTimePos;
	}

	d->scene->mouseMoveEvent(event->pos().x(), mHeight - event->pos().y());
	{
		auto x = event->pos().x();
		auto y = event->pos().y();
		auto scenepos = d->scene->screenPos2ScenePos(XQ::Vec2u(x, mHeight - y));
		auto worldPos = d->scene->getCamera()->ComputeDisplayToWorld(Eigen::Vector3f((float)x / (float)mWidth, (float)(mHeight - y) / (float)mHeight, 0.0f));
		
		auto tip = QString("");
		tip += QString("world:(%1,%2,%3)").arg(worldPos.x(), 0, 'f', 2).arg(worldPos.y(), 0, 'f', 2).arg(worldPos.z(), 0, 'f', 2);
		tip += QString("\nscene:(%1,%2)").arg(scenepos.x(), 0, 'f', 2).arg(scenepos.y(), 0, 'f', 2);
		tip += QString("\nscreen:(%1,%2)").arg(x).arg(y);

		d->screenTextItemMousePos->setText(tip.toStdWString());
		if (!d->scene->getGraphicsItem(d->screenTextItemMousePos->getID())) {
			d->scene->addGraphicsItem(d->screenTextItemMousePos);
		}
	}
	return XOpenGLWidget::mouseMoveEvent(event);
}

void easyPlotWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton ) {
		d->cameraAction = CameraAction::None;
		d->mIsMouseLeftPress = false;
		d->mDrawItemData.item = nullptr;
		d->mDrawItemData.coordArray = nullptr;	
		d->mDrawItemData.type = graphicsItemType::none;
	}

	if (event->button() == Qt::RightButton) {
		d->mIsMouseRightPress = false;
		d->rect->setVisible(false);
		if (d->rectPickEnable) {
			//遍历像素

			if(event->pos() == d->mousePressPos){
				return;
			}
			auto start = mapToGLScreen(d->mousePressPos);
			auto end = mapToGLScreen(event->pos());

			auto x = std::min(start.x(), end.x());
			auto y = std::min(start.y(), end.y());
			int w = start.x() - end.x();
			int h = start.y() - end.y();
			w = std::abs(w);
			h = std::abs(h);

			d->select2dObject.clear();
			d->select3dObject.clear();
			auto selectDatas = d->scene->getBoxSelection(x, y, w, h);
			
			for (int layerNum = 0; layerNum < selectDatas.size(); layerNum++) {
				auto selectData = selectDatas[layerNum];
				for (auto select : selectData) {
					if (select.objectId != 0) {
						d->select3dObject.insert(select.objectId);
						//std::cout << std::dec << "layerNum:" << layerNum << " select object id:" << select.objectId << " primitive id:" << select.primitiveId << std::endl;
					}
				}
			}
			std::cout << "3d框选结果统计\n";
			for (auto s : d->select3dObject) {
				std::cout << std::dec <<  " select object id:" << s << std::endl;
			}

			auto selectDatas2D = d->scene->getBoxSelection2D(x, y, w, h);
			
			for (int layerNum = 0; layerNum < selectDatas2D.size(); layerNum++) {
				auto selectData = selectDatas2D[layerNum];
				for (auto select : selectData) {
					select.objectId;
					select.primitiveId;

					if (select.objectId != 0) {
						d->select2dObject.insert(select.objectId);
						//std::cout << std::dec << "layerNum:" << layerNum << " select object id:" << select.objectId << " primitive id:" << select.primitiveId << std::endl;
					}
				}
			}
			std::cout << "2d框选结果统计\n";
			for (auto s : d->select2dObject) {
				std::cout << std::dec << "select object id:" << s << std::endl;
			}
		}
	}
	return XOpenGLWidget::mouseReleaseEvent(event);
}

void easyPlotWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	d->scene->mouseDoublePressEvent(event->pos().x(), mHeight - event->pos().y(), event->button());
	return XOpenGLWidget::mouseDoubleClickEvent(event);
}

void easyPlotWidget::wheelEvent(QWheelEvent* event)
{	
	auto angle = event->angleDelta().y();

	{
		auto selectId = d->select2dObject.begin();
		if (selectId != d->select2dObject.end()) {
			auto selectIem = d->scene->getGraphicsItem(*selectId);
			if (auto chart = std::dynamic_pointer_cast<XChartItem2>(selectIem)) {
				double factor = 1;
				float t = angle;
				if (std::signbit(t)) {
					factor = 1.1;
				}
				else {
					factor = 1. / 1.1;
				}

				chart->chartSale(factor, factor);;
			}
		}
	}

	//std::cout << std::dec << "滚轮滚动: " << angle << std::endl;
	auto pos = event->pos();
	d->scene->wheelEvent(angle, event->pos().x(), mHeight - event->pos().y());

	return XOpenGLWidget::wheelEvent(event);
}

void easyPlotWidget::resizeEvent(QResizeEvent* event)
{
	XOpenGLWidget::resizeEvent(event);
	
	d->scene->resizeEvent(0, 0, mWidth,  mHeight);

	return XOpenGLWidget::resizeEvent(event);
}

void easyPlotWidget::contextMenuEvent(QContextMenuEvent* event)
{
	// 1. 创建上下文菜单

	if(!isAllowContextmenu())
		return;
	
	QMenu* menu = new QMenu(this);

	// 2. 添加菜单项（QAction）
	QAction* ActFitView = menu->addAction(QIcon(":/icon/fitView.svg"), "FitView3D");
	QAction* ActFitView2D = menu->addAction(QIcon(":/icon/fitView.svg"), "FitView2D");
	QAction* ActDelete = menu->addAction(QIcon(":/icon/deleteObj.svg"), "DeleteSelect");
	QAction* ActDeleteAll = menu->addAction(QIcon(":/icon/clearAll.svg"), "DeleteAll");
	QAction* ActVisibleAll = menu->addAction(QIcon(":/icon/visible.svg"), "visible");
	QAction* ActInvisible = menu->addAction(QIcon(":/icon/invisible.svg"), "invisible");
	QAction* ActSet = menu->addAction(QIcon(":/icon/setting.svg"), "setting");

	// 3. 绑定菜单项的点击信号到槽函数
	connect(ActFitView, &QAction::triggered, this, &easyPlotWidget::slotFitView3D);
	connect(ActFitView2D, &QAction::triggered, this, &easyPlotWidget::slotFitView2D);
	connect(ActDelete, &QAction::triggered, this, &easyPlotWidget::slotDelete);
	connect(ActDeleteAll, &QAction::triggered, this, &easyPlotWidget::slotDeleteAll);
	connect(ActVisibleAll, &QAction::triggered, this, &easyPlotWidget::slotVisibleAll);
	connect(ActInvisible, &QAction::triggered, this, &easyPlotWidget::slotInvisible);
	connect(ActSet, &QAction::triggered, this, &easyPlotWidget::slotSetting);

	// 4. 在鼠标点击位置显示菜单（exec()会阻塞，直到菜单关闭）
	menu->show();
	//d->mouseMoveRealTimePos = event->globalPos();
	menu->exec(event->globalPos()); // event->pos()是相对于当前控件的位置

}

void easyPlotWidget::timerOut()
{
	return;
	#if 0
	static int end = 0;
	static bool add = true;
	if (end == 0) {
		//正向
		add = true;
	}
	if (end == 361) {
		//开始反向
		add = false;
	}
	if (add) {
		d->sphereSource->setEWRange(0, end++);
		d->sphereSource->setEWNum(end);
	}
	else {
		d->sphereSource->setEWRange(0, end--);
		d->sphereSource->setEWNum(end);
	}

	d->sphereSource->setSNRange(30, 150);
	d->sphereSource->setSNNum(180);

	d->sphereSource->Modified();
	#endif
}

DarwItemData easyPlotWidget::createItem(graphicsItemType type)
{
	DarwItemData result;
	if (type == graphicsItemType::rect)
	{
		makeCurrent();
		auto item = makeShareDbObject<XRectItem>();
		item->setPenStyle(XGraphicsItem::PenStyle::DashDotDot);
		item->setRectType((XRectItem::RectType)d->mDrawItemMethod);
		item->setConnectSmoothEnable(false);
		item->setLineWidth(1);
		item->setSingleColor(XQ::Vec4f(0, 1, 1, 1));
		item->setFillColor(XQ::Vec4f(0, 0, 0, 1));
		item->setVisible(true);
		item->setIsFilled(false);

		auto coord = item->getCoordArray();
		item->setCoordArray(coord);
		item->setVisible(false);

		result.type = type;
		result.item = item;
		result.coordArray = coord;


		auto textItem = makeShareDbObject<XTextItem>(nullptr);
		textItem->setSingleColor(XQ::Vec4f(1, 1, 1, 1));
		textItem->setFontSize(20);
		textItem->setText(L"测试一下文字");
		textItem->setVAlignment(XTextItem::VAlign::Middle);
		textItem->setHAlignment(XTextItem::HAlign::Center);
		textItem->setPositionType(XGL::PositionType::local_center);
		textItem->setFixedWidth(2);
		textItem->setIsFixWidth(false);
		textItem->setPosition(0, 0);
		textItem->scale(2,2);
		textItem->setShaderManger(d->scene->getShaderManger());
		item->addChildItem(textItem);
		doneCurrent();
	}

	if (type == graphicsItemType::line)
	{
		makeCurrent();
		auto item = makeShareDbObject<XLineItem>(nullptr);
		item->setLineWidth(2);
		item->setPenStyle(XGraphicsItem::PenStyle::Solid);
		item->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
		item->setVisible(true);

		item->setVisible(false);

		result.type = type;
		result.item = item;
		doneCurrent();
	}
	
	return result;
}

void easyPlotWidget::slotSaveFile( const QString& fileName)
{
	auto s = d->scene->getShape(d->mSelectId);
	/*if(s)
		s->getInput()->writeToFile(fileName.toUtf8().constData());*/
}

void easyPlotWidget::slotOpenFile(const QString& fileName)
{
	
	QFuture<std::shared_ptr<XFileSource>> future = QtConcurrent::run([fileName]()->std::shared_ptr<XFileSource>{
		auto source = makeShareDbObject<XFileSource>();
		source->readFile(fileName.toUtf8().constData());
		return source;
	});

	d->mFutureWatcher->setFuture(future);
	
}

void easyPlotWidget::slotDelete()
{
	{
		auto selects = /*d->scene->getPointSelection(d->mouseMoveRealTimePos.x(), mHeight - d->mouseMoveRealTimePos.y())*/d->select3dObject;
		for (auto select : selects) {
				auto shape = d->scene->getShape(select);
				d->scene->removeShape(shape);
		}
	}
	
	{
		auto selects = /*d->scene->getPointSelection2D(d->mouseMoveRealTimePos.x(), mHeight - d->mouseMoveRealTimePos.y())*/d->select2dObject;
		for (auto select : selects) {
				auto shape = d->scene->getGraphicsItem(select);
				d->scene->removeGraphicsItem(shape);
		}
	}
	
}

void easyPlotWidget::slotDeleteAll()
{
	d->scene->removeAll();
}

void easyPlotWidget::slotVisibleAll()
{
	//显示所有
	std::cout<<"暂未实现\n";//todo
}

void easyPlotWidget::slotInvisible()
{
	//隐藏选中的组件
	{
		auto selects =d->select3dObject;
		for (auto select : selects) {
			auto shape = d->scene->getShape(select);
			shape->setVisible(false);
		}
	}

	{
		auto selects = d->select2dObject;
		for (auto select : selects) {
			auto shape = d->scene->getGraphicsItem(select);
			shape->setVisible(false);
		}
	}
}

void easyPlotWidget::slotCreateCube()
{
	makeCurrent();
	std::shared_ptr<xchamferCubeSource> cubeSource = makeShareDbObject<xchamferCubeSource>();
	std::shared_ptr<XGeometryNode> cubeActor = makeShareDbObject<XGeometryNode>();

	//cubeActor->initResource();
	cubeActor->setInput(cubeSource);

	cubeSource->Modified();

	d->scene->addShape(cubeActor);
	doneCurrent();
}

void easyPlotWidget::slotCreateSphere()
{
	makeCurrent();
	std::shared_ptr<XSphereSource> sphereSource = makeShareDbObject<XSphereSource>();
	std::shared_ptr<XGeometryNode> sphereActor = makeShareDbObject<XGeometryNode>();

	//sphereActor->initResource();
	sphereActor->setInput(sphereSource);
	sphereActor->scale(10,10,10);
	sphereSource->setDirection(XSphereSource::Direaction::horizontal);
	sphereSource->setEWRange(0, 360);
	sphereSource->setSNRange(0, 180);
	sphereSource->setEWNum(360);
	sphereSource->setSNNum(180);
	sphereSource->Modified();
	d->scene->addShape(sphereActor);
	doneCurrent();
}

void easyPlotWidget::slotCreateCone()
{
	makeCurrent();
	std::shared_ptr<XConeSource> coneSource = makeShareDbObject<XConeSource>();
	std::shared_ptr<XGeometryNode> coneActor = makeShareDbObject<XGeometryNode>();
	//coneActor->initResource();
	coneActor->setInput(coneSource);
	coneActor->translate(0, 0, 4);
	coneActor->scale(2, 2, 2);
	d->scene->addShape(coneActor);
	coneActor->setSingleColor(XQ::Vec4f(0,1,0,1));
	coneActor->setColorMode(ColorMode::SingleColor);
	doneCurrent();
}

void easyPlotWidget::slotSetting()
{
	auto selects = d->select2dObject;
	if (selects.size() == 1) {
		auto shape = d->scene->getGraphicsItem(*selects.begin());

		if (auto text = std::dynamic_pointer_cast<XTextItem>(shape)) {
			FontSetDlg::TextSetInfo info;
			info.content = QString::fromStdWString(text->getText());
			info.alignH = (int)text->getHAlignment() - 1;
			info.alignV = (int)text->getVAlignment() - 1;
			info.x = text->getPositionByOrientation().x();
			info.y = text->getPositionByOrientation().y();
			info.isFixed = text->isFixWidth();
			info.fixWidth = text->getFixedWidth();
			info.color = text->getSingleColor();
			info.tackMode = (int)text->getPositionType();
			info.refOrigin = (int)text->getOrientation();
			static FontSetDlg *dlg = nullptr;
			if(dlg == nullptr)
				dlg = new FontSetDlg(info,this);
			else
				dlg->setTexInfo(info);

			dlg->addFun([text]() {
				auto info = dlg->getTextInfo();
				text->setText(info.content.toStdWString());
				text->setHAlignment((XTextItem::HAlign)(info.alignH + 1));
				text->setVAlignment((XTextItem::VAlign)(info.alignV + 1));
				text->setIsFixWidth(info.isFixed);
				text->setFixedWidth(info.fixWidth);
				text->setSingleColor(info.color);
				text->setPositionType(XGL::PositionType(info.tackMode));
				text->setOrientation(XGL::Orientation(info.refOrigin));
				text->setPosition(info.x, info.y);
				});
			dlg->raise();
			dlg->show();
		}

		else if (auto item = std::dynamic_pointer_cast<XGridItem>(shape)) {
			GridSetParam info;
			info.gridNum = item->getGridNum();
			info.gridSpace = item->getGridSpace();
			info.rangex_min = item->getOrigin().x();
			auto scales =item->gridGetSale();
			auto origin = item->getOrigin();

			info.rangex_min = origin.x();
			info.rangex_max = origin.x() + scales.x();

			info.rangey_min = origin.y();
			info.rangey_max = origin.y() + scales.y();

			static GridSetDlg* dlg = nullptr;
			if (dlg == nullptr) {
				dlg = new GridSetDlg(info, this);
				connect(dlg,&GridSetDlg::sigGridSetParam,this,[item](const GridSetParam& parm){
					auto sx =parm.rangex_max - parm.rangex_min;
					auto sy =parm.rangey_max - parm.rangey_min;
					item->setOrigin(XQ::Vec2f(parm.rangex_min, parm.rangey_min));
					item->gridSetSale(sx,sy);
					item->setGridNum( parm.gridNum);
					item->setGridSpace(parm.gridSpace);
				});
			}
			else
				dlg->setGridSetParam(info);

			dlg->raise();
			dlg->show();
		}

		else {
			QMessageBox::warning(this, "Warning", "No support for this item.");
		}
	}
}

void easyPlotWidget::slotFileLoadFinished()
{
	makeCurrent();
	//创建一个新的shape
	auto shape = makeShareDbObject<XGeometryNode>();
	//shape->initResource();

	auto source =d->mFutureWatcher->result();

	shape->setInput(source);
	shape->setColorMode(ColorMode::SingleColor);
	shape->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
	shape->translate(4, 2, 0);
	shape->scale(2, 1, 1);

	d->scene->addShape(shape);
	doneCurrent();

	auto boundbox = shape->getBoundBox();
	d->scene->getCamera()->resetCamera((double*)&boundbox);
}

void easyPlotWidget::slotAnyTaskFinished()
{
	//加载文件
	QString str;
	QString filePath = qApp->applicationDirPath()+"/tip.txt";
	QFile file1(filePath);
	if (!file1.open(QIODevice::ReadWrite | QIODevice::Text)) {
		return;
	}
	str = file1.readAll();
	file1.close();
	d->screenTextItem->setText(str.toStdWString());

	auto num = xfreetype::Instance()->computeLineNum(str.toStdWString(),
	d->screenTextItem->getFontSize(), d->screenTextItem->getFixedWidth(), d->screenTextItem->isFixWidth());
	
	QStringList lineNumber; 
	
	for(int i=0;i< num;i++){
		lineNumber.append(QString("%1").arg(i));
	}

	double maxWidth = 0;
	for(auto s:lineNumber)
		maxWidth = std::max(maxWidth, xfreetype::Instance()->computeLineStrWidth(s.toStdWString(), d->screenTextItemLine->getFontSize()));

	auto mergeStr = lineNumber.join('\n');

	d->screenTextItemLine->setText(mergeStr.toStdWString());

	std::cout << "行数：" <<std::dec<< num << std::endl;
}

void easyPlotWidget::slotRectPickEnable(bool flag)
{
	d->rectPickEnable = flag;
}

void easyPlotWidget::slotSetDarwItemType(int type,int method)
{
	d->mDrawItemType = (graphicsItemType)type;
	d->mDrawItemMethod = method;
}

void easyPlotWidget::slotFitView2D()
{
	d->scene->fitView2D();
}

void easyPlotWidget::slotFitView3D()
{
	d->scene->fitView3D();
}

void easyPlotWidget::updateItem()
{
	if(!d->mIsMouseLeftPress)
		return;
	auto firstPos = d->scene->screenPos2ScenePos(mapToGLScreen( d->mousePressPos));
	auto secondPos = d->scene->screenPos2ScenePos( mapToGLScreen(d->mouseMoveRealTimePos));
	

	if (d->mDrawItemData.item) {

		//d->mDrawItemData.item->resetTransform();
		d->mDrawItemData.item->setVisible(true);
		if (d->mDrawItemType == graphicsItemType::rect) {
			auto rect = std::dynamic_pointer_cast<XRectItem>(d->mDrawItemData.item); {
				rect->resetTransform();
				auto scalex = (firstPos - secondPos).x();
				auto scaley = (firstPos - secondPos).y();
				auto center = (firstPos + secondPos) * 0.5;
				rect->translate(center.x(), center.y());
				rect->scale(abs(0.5*scalex), abs(0.5*scaley));
			}
		}
		if (d->mDrawItemType == graphicsItemType::line) {
			
			auto line = std::dynamic_pointer_cast<XLineItem>(d->mDrawItemData.item);
			line->resetTransform();
			line->setLine(firstPos, secondPos);
		}
	}
}

void easyPlotWidget::slotShowGrid2D(bool flag)
{
	d->scene->setGrid2dVisible(flag);
}

void easyPlotWidget::slotShowGrid3D(bool flag)
{
	d->scene->setGrid3dVisible(flag);
}

void easyPlotWidget::slotShowAxis3D(bool flag)
{
	d->scene->setAxis3dVisible(flag);
}

void easyPlotWidget::slotAddLine2D(int curveType)
{
	//1 正弦波 2 三角波 3 矩形波
	//创建一条曲线
	makeCurrent();
	auto item = makeShareDbObject<XPolyline>(nullptr);
	std::wstring name = L"曲线";
	name.append(std::to_wstring(XQ::math::randon<int>(0, 100)));
	item->addAttribute(L"Name", name);
	item->setLineWidth(3);
	item->setFixedLine(true);
	item->setPenStyle(XGraphicsItem::PenStyle::Solid);

	//生成随机颜色
	auto r =XQ::math::randon_color();
	auto g = XQ::math::randon_color();
	auto b= XQ::math::randon_color();
	item->setSingleColor(XQ::Vec4f(r,g, b, 1));
	//item->initiallize();
	item->setVisible(true);

	
	auto curveData = makeShareDbObject<XFloatArray>();
	curveData->setComponent(3);

	//生成正弦波
	auto xoffset = XQ::math::randon<float>(-200, 200);
	auto yoffset = XQ::math::randon<float>(10, 50);

	if (curveType == 1) {
		
		auto fre = XQ::math::randon<float>(0.05, 0.1);
		int num = 200;
		curveData->setNumOfTuple(num);
		//x随机偏移
		//y随机偏移
		//std::vector<int> x{ 2500,-2500,-2500,-500,-500,-1500,-1500,1500,1500,500,500,2500,2500 };
		//std::vector<int> y{ 5000,5000,4000,4000,1000,1000,0,0,1000,1000,4000,4000,5000 };
		for (int i = 0; i < num; i++) {
			auto x = i;
			auto y = 100 * sin(fre * x);
			auto z = 0;
			curveData->setTuple(i, x, y, z);
		}
	}
	//生成三角波
	if (curveType == 2) {
		//设周期T
		double T =30;	
		auto fre = XQ::math::randon<float>(0.05, 0.1);
		int num = 50; //表示周期

		//三角波
		double peak_y = 30;
		double peak_x = 0.5*T;
		curveData->setNumOfTuple(num*2);
		for (int i = 0; i < num; i++) {
			auto x0 = i*T-0.5*num*T;
			auto y0 =0;

			auto x1 = x0 + peak_x;
			auto y1 = peak_y;

			curveData->setTuple(2*i+0, x0, y0, 0);
			curveData->setTuple(2*i+1, x1, y1, 0);
		}
	}
	//item->translate(xoffset, yoffset);
	curveData->Modified();


	item->setCoordArray(curveData);
	
	bool flag = true;
	auto selectId = d->select2dObject.begin();
	if (selectId != d->select2dObject.end()) {
		auto selectIem = d->scene->getGraphicsItem(*selectId);
		if (auto chart = std::dynamic_pointer_cast<XChartItem2>(selectIem)) {
			chart->addPolyline(item);
			chart->fitView();
			flag = false;
		}
	}

	if (flag) {
		d->scene->addGraphicsItem(item);
	}

	doneCurrent();
}

void easyPlotWidget::slotAddChart()
{
	makeCurrent();
	#if 1
	{
		auto chart = makeShareDbObject<XChartItem2>();
		chart->setVisible(true);

		auto tx = XQ::math::randon<double>(-200, 200);
		auto ty = XQ::math::randon<double>(-200, 200);
		auto sx = XQ::math::randon<double>(400, 500);
		auto sy = XQ::math::randon<double>(300, 400);

		chart->translate(tx, ty);

		auto angle = XQ::math::randon<double>(-90, 90);
		//chart->rotate(angle);

		chart->scale(sx, sy);

		chart->setTtitle(L"图表标题测试" );

		d->scene->addGraphicsItem(chart);
	}
	#else
	{
		auto grid = makeShareDbObject<XGridItem>();
		grid->gridTranslate(-1,-1);
		grid->gridSetSale(200,100);
		grid->setIsScreenGrid(false);
		grid->setOrigin(XQ::Vec2f(-20,-20));

		//网格的大小
		grid->scale(100, 50);

		d->scene->addGraphicsItem(grid);

	}
	#endif

	doneCurrent();
}

void easyPlotWidget::slotAddBar()
{
	//创建一条曲线
	makeCurrent();
	#if 1
	auto item = makeShareDbObject<XBarItem>(nullptr);
	item->setLineWidth(1);
	item->setPenStyle(XGraphicsItem::PenStyle::Dash);

	//生成随机颜色
	auto r = XQ::math::randon_color();
	auto g = XQ::math::randon_color();
	auto b = XQ::math::randon_color();
	item->setSingleColor(XQ::Vec4f(r, g, b, 1));
	item->setFillColor(XQ::Vec4f(r, g, b, 1));
	//item->initiallize();
	item->setVisible(true);

	//数据
	auto curveData = makeShareDbObject<XFloatArray>();
	curveData->setComponent(3);

	auto xoffset = XQ::math::randon<float>(-200, 200);
	auto yoffset = XQ::math::randon<float>(10, 50);
	auto fre = XQ::math::randon<float>(0.05, 0.1);
	int num = 10000;
	curveData->setNumOfTuple(num);
	//x随机偏移
	//y随机偏移

	//柱坐标系应该规范多条数据X轴坐标相同，都为整数
	for (int i = 0; i < num; i++) {
		auto x =-0.5*num + i;
		auto y = XQ::math::randon<int>(10, 200);
		auto z = 0;
		curveData->setTuple(i, x, y, z);
	}
	item->translate(xoffset,0);
	curveData->Modified();
	item->setCoordArray(curveData);

	bool flag = true;
	auto selectId = d->select2dObject.begin();
	if (selectId != d->select2dObject.end()) {
		auto selectIem = d->scene->getGraphicsItem(*selectId);
		if (auto chart = std::dynamic_pointer_cast<XChartItem>(selectIem)) {
			chart->addPolyline(item);
			flag = false;
		}
	}

	if (flag) {
		d->scene->addGraphicsItem(item);
	}
	#endif

	//for (int i = 0; i < 1; i++) {
	//	auto item = makeShareDbObject<XLegendRowItem>();
	//	item->setSingleColor(XQ::Vec4f( 1,0,0,0));
	//	item->setVisible(true);
	//	item->setChecked(true);
	//	item->translate(0,30*i+10);
	//	//item->scale(200, 30);
	//	d->scene->addGraphicsItem(item);
	//}

	doneCurrent();
}

void easyPlotWidget::slotAddText()
{
	makeCurrent();
	{
		auto item = makeShareDbObject<XTextItem>(nullptr);
		//item->initiallize();
		item->setVisible(true);
		item->setVAlignment(XTextItem::VAlign::Bottom);
		item->setHAlignment(XTextItem::HAlign::Center);

		
		item->setFontSize(36);
		item->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
		item->setText(L"请输入一段文字");
		item->setPositionType(XGL::PositionType::local_center);
		item->setOrientation(XGL::Orientation::left_bottom);

		item->setPosition(100, 100);
		d->scene->addGraphicsItem(item);
	}
	doneCurrent();
}

void easyPlotWidget::slotGenerateFontTextures()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		qApp->applicationDirPath(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
		xfreetype::Instance()->generateFontTextures(dir, true, false);
}

void easyPlotWidget::slotGenerateFontSdf()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		qApp->applicationDirPath(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if(!dir.isEmpty())
		xfreetype::Instance()->generateFontSdf(dir, true, false);
}

void easyPlotWidget::slotScreenTextVisible(bool flag)
{
	d->screenTextItemLine->setVisible(flag);
	d->screenTextItem->setVisible(flag);
	if (!d->scene->getGraphicsItem(d->screenTextItem->getID())) {
		d->scene->addGraphicsItem(d->screenTextItem);
	}
	if (!d->scene->getGraphicsItem(d->screenTextItemLine->getID())) {
		d->scene->addGraphicsItem(d->screenTextItemLine);
	}
}

void easyPlotWidget::slotScreenShot()
{
	//d->scene->grabFramebuffer();
	//return;
	static ScreenShotDlg* dlg = nullptr;
	if (dlg == nullptr) {

		dlg = new ScreenShotDlg(0,0,d->scene->getViewportWidth(),d->scene->getViewportHeight(), 
		0,0,d->scene->getViewportWidth(),d->scene->getViewportHeight(), this);

		connect(dlg, &ScreenShotDlg::sigScreenShot, this, [this](int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, QString fileName){
			auto image_data = d->scene->grabFramebuffer(src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h);
			image_data->flip();
			auto textureWidth = image_data->getCol();
			auto textureHeight = image_data->getRow();
			QImage image(image_data->data(0, 0), textureWidth, textureHeight, textureWidth * 4/*指定行宽，避免对齐问题*/, QImage::Format_RGBA8888);
			image.save(fileName);
		});
	}
		
	dlg->raise();
	dlg->show();
}

//创建坐标轴
void easyPlotWidget::slotAxis2D()
{
	makeCurrent();
	{
		auto axis = makeShareDbObject<XAxisItem>();

		axis->setVisible(true);

		axis->getLine()->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
		axis->getLine()->setFixedLine(true);
		axis->getLine()->setLineWidth(2);
		axis->getLine()->setPositionType(XGL::PositionType::local_complete);
		axis->getLine()->setOrientation(XGL::Orientation::left_bottom);
		axis->getLine()->translate(100,100);
		axis->getLine()->scale(100,1);

		//axis->translate(0,100);
		//axis->scale(100,1);
		axis->setRange(0,100);
		d->scene->addGraphicsItem(axis);
	}
	doneCurrent();
}

unsigned int combine(float depth, unsigned char stencial) {
	auto maxUint = std::numeric_limits<unsigned int>::max();
	maxUint = maxUint >>8;
	unsigned int udepth = maxUint * depth;
	udepth = udepth << 8;
	auto ret = udepth  + stencial;
	return ret;
}

void easyPlotWidget::slotFboTest()
{
	{
		makeCurrent();
#if 0
		auto strs =XOpenGLFuntion::xglGetExtensions();
		bool isSupport = XOpenGLFuntion::xisExtensionSupported("GL_ARB_get_texture_sub_image");
		
		int rowNUm = 10;
		int colNum = 20;
		auto cpuData = makeShareDbObject<XUCharArray2D>();
		cpuData->setComponent(1);
		cpuData->setDimensions(colNum, rowNUm);

		std::cout<<"[1]\n从CPU读取数据到纹理\n";
		{
			for (int row = 0; row < rowNUm; row++) {
				for (int col = 0; col < colNum; col++) {
					*cpuData->data(row, col) = row * colNum + col;
				}
			}

			std::cout << "cpuData\n";
			cpuData->dump(true);
		}

		auto oldUnpackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, 1);
		auto oldUnpackRowlegth = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_row_length, 10);
		auto oldUnpackSkipRows = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_skip_rows, 1);
		auto oldUnpackSkipPixels = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_skip_pixels, 1);

		int textureWidth = 5;
		int textureHeight = 4;
		auto texture = makeShareDbObject<XOpenGLTexture>();
		texture->setTarget(XOpenGLTexture::Target2D);
		texture->setInternalFormat(XOpenGLTexture::TextureFormat::R8_UNorm);
		texture->create();
		texture->bind(); 

		texture->setData(textureWidth, textureHeight,0,XOpenGLTexture::PixelFormat::Red, XOpenGLTexture::PixelType::UInt8, cpuData->data(0,0));

		//恢复配置
		oldUnpackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldUnpackAlignment);
		oldUnpackRowlegth = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_row_length, oldUnpackRowlegth);
		oldUnpackSkipRows = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_skip_rows, oldUnpackSkipRows);
		oldUnpackSkipPixels = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_skip_pixels, oldUnpackSkipPixels);

		{
			auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, 1);

			auto pbo = texture->map();
			auto* map_data = (unsigned char*)pbo->map(XOpenGLBuffer::ReadOnly);
			auto cpuData = makeShareDbObject<XUCharArray2D>();
			cpuData->setComponent(1);
			cpuData->setDimensions(textureWidth,textureHeight);
			cpuData->setData(0, 0, textureWidth, textureHeight, map_data);
			std::cout << "GPU2CPU\n";
			cpuData->dump(true);
			pbo->unmap();

			oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldPackAlignment);
		}

		std::cout << "[2]\n从纹理读取数据到CPU\n";
		{
			auto cpuData = makeShareDbObject<XUCharArray2D>();
			cpuData->setComponent(1);
			cpuData->setDimensions(10, 10);
			std::cout<<"原始CPU分配足够的空白内存\n";
			cpuData->dump(true);

			oldUnpackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, 1);
			oldUnpackRowlegth = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, 10);
			oldUnpackSkipRows = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, 3);
			oldUnpackSkipPixels = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, 3);

			auto pbo = texture->map(10,10,3,3);
			auto tmp = pbo->map2cpu();
			cpuData->setData(0, 0, 10, 10, tmp->data(0));
			std::cout << "GPU2CPU\n";
			cpuData->dump(true);
	
		}

		std::cout << "[3]\n读取纹理子区域数据到CPU\n";
		{
			auto cpuData = makeShareDbObject<XUCharArray2D>();
			cpuData->setComponent(1);
			cpuData->setDimensions(10, 10);
			std::cout << "原始CPU分配足够的空白内存\n";
			cpuData->dump(true);

			auto fbo = makeShareDbObject<XOpenGLFramebufferObject>();
			fbo->create();
			fbo->bind();
			fbo->setWidth(10);
			fbo->setHeight(10);
			fbo->addAttachment(XOpenGLFramebufferObject::Color,texture);
			bool flag =fbo->isComplete();
			if (flag) {
				auto s1 = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, 1);
				auto s2 = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, 10);
				auto s3 = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, 3);
				auto s4 = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, 3);

				XOpenGLFuntion::xglReadPixels(1,1,3,2,XOpenGL::TextureExternalFormat::Red,XOpenGL::DataType::unsigned_byte, cpuData->data(0,0));

				std::cout << "xglReadPixels\n";
				cpuData->dump(true);

				XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, s1);
				XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_row_length, s2);
				XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_rows, s3);
				XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_skip_pixels, s4);
			}

			fbo->release(XOpenGL::FrameBufferType::framebuffer);
		}
		#endif
		std::cout << "[1]\nXDataArray3D测试\n";
		{
			int rowNUm = 4;
			int colNum = 5;
			auto cpuData = makeShareDbObject<XUIntArray2D>(colNum,rowNUm,1);

			std::cout << "[1]\n从CPU读取数据到纹理\n";
			{
				*cpuData->data(0, 0) = 0x12345601;
				*cpuData->data(0, 1) = 0x23456702;

				std::cout << "cpuData\n";
				cpuData->dump(true);
			}

			int w = 20;		//每一行5个颜色数据
			int h = rowNUm;
			int z = 2;
			auto array3D =makeShareDbObject<XUCharArray3D>(w,h,z,1);
			array3D->memCopy(0,cpuData);
			array3D->forEach(1, [](unsigned char* data) {
				*data+=0x123456;
				});
			array3D->dump(true);

			auto textureArry2d =makeShareDbObject<XOpenGLTexture>();
			textureArry2d->setTarget(XOpenGLTexture::Target2DArray);
			textureArry2d->setInternalFormat(XOpenGLTexture::RGBA8_UNorm);
			textureArry2d->create();
			textureArry2d->bind();

			auto oldUnpackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, 1);
			textureArry2d->setData(w,h,0,XOpenGLTexture::PixelFormat::RGBA,XOpenGLTexture::PixelType::UInt8,{ array3D->data(0),array3D->data(1) });
			oldUnpackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldUnpackAlignment);

			
			std::cout<<"回读二维数组纹理\n";
			auto pbo =textureArry2d->map(1);
			auto array1d = pbo->map2cpu();
			{
				auto array3D = makeShareDbObject<XUCharArray3D>();
				array3D->setComponent(1);
				array3D->setDimensions(w, h, z);
				array3D->memCopy(array1d);
				array3D->dump();
			}

		}

		std::cout << "[2]\n深度模板附件测试\n";
		{
			XQ::print("深度附件测试...");
			{
				//5*4的二维纹理数组
				auto row = 4;
				auto col = 5;
				auto cpuDepthData = makeShareDbObject<XFloatArray2D>(5,4,1);
				cpuDepthData->setRowData(
					0.1, 0.2, 0.3, 0.4, 1.0,
					0.5, 0.6, 0.7, 0.8, 1.0,
					0.8, 0.9, 0.8, 0.6, 1,
					0.5, 0.4, 0.3, 0.2, 1.0
				);

				XQ::print("cpu端预设深度");
				cpuDepthData->dump();

				XQ::print("创建深度纹理");
				auto depthTexture = makeShareDbObject< XOpenGLTexture>();
				depthTexture->setInternalFormat(XOpenGLTexture::D32);
				depthTexture->setTarget(XOpenGLTexture::Target2D);
				depthTexture->create();
				depthTexture->bind();

				depthTexture->setData(col, row, 0, XOpenGLTexture::PixelFormat::Depth, XOpenGLTexture::PixelType::Float32, cpuDepthData->data());

				XQ::print("读取深度纹理");
				auto pbo = depthTexture->map(1);
				auto pbo2cpuData = pbo->map2cpu();

				auto gpuDepthData = makeShareDbObject<XFloatArray2D>();
				gpuDepthData->setComponent(1);
				gpuDepthData->setDimensions(5, 4);
				gpuDepthData->memCopy(pbo2cpuData);
				gpuDepthData->dump();
				depthTexture->release();
			}
		
			XQ::print("模板附件测试...");
			{
				//5*4的二维纹理数组
				auto row = 4;
				auto col = 5;
				auto cpuStencilData = makeShareDbObject<XUCharArray2D>(5,4,1);
				cpuStencilData->setRowData(
					1, 2, 3, 4, 5,
					6, 7, 8, 9, 10,
					11, 12, 13, 14, 15,
					16, 17, 18, 19, 20
				);

				XQ::print("cpu端预设模板");
				cpuStencilData->dump();

				XQ::print("创建模板纹理");
				auto depthTexture = makeShareDbObject< XOpenGLTexture>();
				depthTexture->setInternalFormat(XOpenGLTexture::TextureFormat::S8);
				depthTexture->setTarget(XOpenGLTexture::Target2D);
				depthTexture->create();
				depthTexture->bind();

				depthTexture->setData(col, row, 0, XOpenGLTexture::PixelFormat::Stencil, XOpenGLTexture::PixelType::UInt8, cpuStencilData->data());

				XQ::print("读取模板纹理");
				auto pbo = depthTexture->map(1);
				auto pbo2cpuData = pbo->map2cpu();

				auto gpuDepthData = makeShareDbObject<XUCharArray2D>();
				gpuDepthData->setComponent(1);
				gpuDepthData->setDimensions(5, 4);
				gpuDepthData->memCopy(pbo2cpuData);
				gpuDepthData->dump();
				depthTexture->release();
			}

			XQ::print("深度模板附件测试...");
			{
				auto row = 4;
				auto col = 5;
				auto cpuDepthData = makeShareDbObject<XUIntArray2D>(5,4,1);
				cpuDepthData->setRowData(
					 combine(0.1,1),combine(0.2,2),combine(0.3,3),combine(0.4,4),combine(0.5,5) ,
					 combine(0.6,6),combine(0.7,7),combine(0.8,8),combine(0.9,9),combine(0.8,10) ,
					 combine(0.7,11),combine(0.6,12),combine(0.5,13),combine(0.4,14),combine(0.3,15) ,
					 combine(0.2,16),combine(0.1,17),combine(0.0,18),combine(0.1,19),combine(0.2,20) 
				);
				XQ::print("cpu端预设深度模板");
				std::cout << std::hex;
				cpuDepthData->dump();
				std::cout << std::dec;

				XQ::print("创建深度模板纹理");
				auto depthTexture = makeShareDbObject< XOpenGLTexture>();
				depthTexture->setInternalFormat(XOpenGLTexture::D24S8);
				depthTexture->setTarget(XOpenGLTexture::Target2D);
				depthTexture->create();
				depthTexture->bind();

				depthTexture->setData(col, row, 0, XOpenGLTexture::PixelFormat::DepthStencil, XOpenGLTexture::PixelType::UInt32_D24S8, cpuDepthData->data());

				XQ::print("读取深度模板纹理");
				auto pbo = depthTexture->map(1);
				auto pbo2cpuData = pbo->map2cpu();

				auto gpuDepthData = makeShareDbObject<XUIntArray2D>(5,4,1);
				gpuDepthData->memCopy(pbo2cpuData);
				std::cout<<std::hex;
				gpuDepthData->dump();
				std::cout << std::dec;
				depthTexture->release();

				XQ::print("读取深度模板纹理分离读取");
				{	
					int w=5,h=4;
					auto seprateDepthFloat = makeShareDbObject<XFloatArray2D>(w,h,1);

					auto seprateDepthU32 = makeShareDbObject<XUIntArray2D>(w,h,1);

					auto seprateStencil = makeShareDbObject<XUCharArray2D>(w,h,1);

					auto fbo =makeShareDbObject<XOpenGLFramebufferObject>(w,h);
					fbo->create();
					fbo->bind();
					fbo->addAttachment(XOpenGLFramebufferObject::CombinedDepthStencil,depthTexture);
					if(fbo->isComplete()){
						std::cout<<"fbo is complete\n";
					}else
						std::cout << "fbo no complete\n";
					auto oldAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment,1);
					XOpenGLFuntion::xglReadPixels(0, 0, 5, 4, XOpenGL::TextureExternalFormat::Depth, XOpenGL::DataType::float_, seprateDepthFloat->data());
					//GL_UNSIGNED_INT读取24位深度时，深度值被左对齐到32位整数的高24位，低8位是未定义的垃圾数据
					XOpenGLFuntion::xglReadPixels(0, 0, 5, 4, XOpenGL::TextureExternalFormat::Depth, XOpenGL::DataType::unsigned_int, seprateDepthU32->data());
					XOpenGLFuntion::xglReadPixels(0, 0, 5, 4, XOpenGL::TextureExternalFormat::Stencil, XOpenGL::DataType::unsigned_byte, seprateStencil->data());
					oldAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::pack_alignment, oldAlignment);

					XQ::print("分离的浮点深度");
					seprateDepthFloat->dump();
					
					XQ::print("分离的UInt深度");
					std::cout << std::hex;
					seprateDepthU32->dump();
					std::cout << std::dec;

					XQ::print("分离的模板");
					seprateStencil->dump();
				}

			}
		}
		doneCurrent();
		return;
	}


	#if 0
	static FboTest* dlg = nullptr;
	if (dlg == nullptr) {

		dlg = new FboTest( this);

		connect(dlg, &FboTest::sigTest, this, [this](int fboWidth, int fboHeight, bool depthEnable, bool multisampleEnable, int samples, int AttachMode) {
			auto image_datas = d->scene->renderFbo(fboWidth, fboHeight, depthEnable, multisampleEnable, samples, AttachMode);
			for(auto image_data:image_datas)
				image_data->flip();

			auto width = image_datas[0]->getCol();
			auto height = image_datas[0]->getRow();
			QImage image(image_datas[0]->data(0, 0), width, height, width * 4/*指定行宽，避免对齐问题*/, QImage::Format_RGBA8888);
			dlg->setColorImage(image);
			if (image_datas.size() == 2) {
				QImage image(image_datas[1]->data(0, 0), width, height, width /*指定行宽，避免对齐问题*/, QImage::Format_Grayscale8);
				dlg->setDepthImage(image);
			}
			});
	}

	dlg->raise();
	dlg->show();
	#endif
}

void easyPlotWidget::slotComputeShaderTest()
{
	makeCurrent();
	auto computeShader = makeShareDbObject<XComputeShader>();
	computeShader->loadComputeShader(XQ::ShareVar::instance().currentExeDir + "/easyPlot/" + "compute.cs");
	
	computeShader->use();
	//准备数据，创建一个SSBO，并分配数据
	auto ssbo = makeShareDbObject<XOpenGLBuffer>();
	
	ssbo->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);
	ssbo->setUsagePattern(XOpenGLBuffer::StaticDraw);
	ssbo->create();
	ssbo->bind();

	auto d =makeShareDbObject<XFloatArray>();
	d->setNumOfTuple(256);

	for (int i = 0; i < 256; i++) {
		d->setTuple(i,i);
	}

	ssbo->allocate( d->data(0), 256);

	ssbo->setBufferBindIdx(1);

	ssbo->release();

	//派发任务
	computeShader->dispatchCompute(1, 1, 1);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	//异步等待GPU执行结束
	auto glSyncObject = XOpenGLFuntion::xglFenceSync(XOpenGL::SyncFlags::none);
	auto  shareContext = getContext()->createOrgetShareContext();
	std::thread t([shareContext, glSyncObject, computeShader, ssbo](){
		if (!shareContext->makeCurrent()) {
			return;
		}

		auto waitResult = XOpenGLFuntion::xglClientWaitSync(glSyncObject, XOpenGL::SyncFlags::none, std::numeric_limits<unsigned long long>::max());
		if (waitResult == XOpenGL::SyncStatus::SyncStatusAlreadySignaled ||
			waitResult == XOpenGL::SyncStatus::SyncStatusConditionSatisfied)
		{
			std::cout<<"compute finished!\n";
			//读取SSBO数据
			auto ptr = (float*)ssbo->map(XOpenGLBuffer::ReadOnly);

			auto d = makeShareDbObject<XFloatArray>();
			d->setNumOfTuple(256);
			
			for (int i = 0; i < 256; i++) {
				d->setTuple(i,ptr[i]);
			}
			//
		}
		else {
			std::cout << "wait sync failed\n";
		}

		XOpenGLFuntion::xglDeleteSync(glSyncObject);

		shareContext->doneCurrent();
	});
	t.detach();

	doneCurrent();
}