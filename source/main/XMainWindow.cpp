#include "XMainWindow.h"
#include "ui_XMainWindow.h"
#include <qdockwidget.h>
#include <QTextEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <easyPlot/XEasyPlotWidget.h>
#include <xmvd/XObjectInspectorView.h>
#include <qtabwidget.h>

#include <render/XRender.h>
#include <lib05_shape/XRenderNode.h>

class XMainWindow::Internal {
public:
	QTabWidget* leftTab = nullptr;
	QTabWidget* rightTab = nullptr;
	QTabWidget* botTab = nullptr;
	XEasyPlotWidget *plotWidget = nullptr;

	XObjectInspectorView *objectInspector = nullptr;
};

XMainWindow::XMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::XMainWindow), mData(new Internal)
{
    ui->setupUi(this);
	BuildUI();
	InitView();
}

XMainWindow::~XMainWindow()
{
    delete ui;
}

void XMainWindow::BuildUI()
{
	// 中央区域：主显示内容
	mData->plotWidget = new XEasyPlotWidget(this);
	this->centralWidget()->setLayout(new QHBoxLayout);
	this->centralWidget()->layout()->addWidget(mData->plotWidget);

	// 左侧 Dock
#if 0
	QDockWidget* leftDock = new QDockWidget("left", this);
	leftDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mData->leftTab = new QTabWidget(this);
	leftDock->setWidget(mData->leftTab);
	this->addDockWidget(Qt::LeftDockWidgetArea, leftDock);
#endif

	// 右侧 Dock
	QDockWidget* rightDock = new QDockWidget("right", this);
	rightDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mData->rightTab = new QTabWidget(this);
	rightDock->setWidget(mData->rightTab);
	this->addDockWidget(Qt::RightDockWidgetArea, rightDock);
	this->resizeDocks({ rightDock },{50},Qt::Horizontal);
	// 底部 Dock
#if 0
	QDockWidget* bottomDock = new QDockWidget("bottom", this);
	bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	mData->botTab = new QTabWidget(this);
	bottomDock->setWidget(mData->botTab);
	this->addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
#endif

	InitLeftTab();
	InitRightTab();
	InitBotTab();
}

void XMainWindow::InitLeftTab()
{
}

void XMainWindow::InitRightTab()
{
	mData->objectInspector = new XObjectInspectorView(this);
	mData->rightTab->setTabPosition(QTabWidget::South);
	mData->rightTab->addTab(mData->objectInspector, "Attribute");
}

void XMainWindow::InitBotTab()
{
}

void XMainWindow::InitView()
{
	xsig::connect(mData->plotWidget,&XEasyPlotWidget::sigRenderAdd,[this](sptr<XRender> render){
		xsig::connect(render,&XRender::SigRenderNodeSelected,[this](sptr<XRenderNode> node){
			auto parent = node;
			while (auto p = parent->getRenderNodeParent()) {
				parent = p;
			}
			mData->objectInspector->setRootObject(parent);
		});
	});

	mData->plotWidget->test2();
}
