#include <QHBoxLayout> 
#include <QWidget>
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog> 
#include<easyPlot/easyPlotWidget.h>

#include <qtoolbutton.h>
#include <lib05_shape/XRectItem.h>

easyPlotWidget * easyPlot = nullptr;

void setDarwType(render::graphicsItemType type, int method, easyPlotWidget* p, bool flag) {
	if (flag)
		p->slotSetDarwItemType((int)type,method);
	else
		p->slotSetDarwItemType((int)render::graphicsItemType::none,method);
}


MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	easyPlot = new easyPlotWidget(this);
	ui->tab_easyPlot->layout()->addWidget(easyPlot);

	ui->stackedWidget->setVisible(false);

	connect(ui->ActsaveMesh, &QAction::triggered, this, &MainWindow::slotSaveMesh);
	connect(ui->ActOpenMesh, &QAction::triggered, this, &MainWindow::slotOpenMesh);

	connect(ui->ActCone, &QAction::triggered, easyPlot, &easyPlotWidget::slotCreateCone);
	connect(ui->ActCube, &QAction::triggered, easyPlot, &easyPlotWidget::slotCreateCube);
	connect(ui->ActSphere, &QAction::triggered, easyPlot, &easyPlotWidget::slotCreateSphere);


	connect(ui->Act2DFit, &QAction::triggered, easyPlot, &easyPlotWidget::slotFitView2D);
	connect(ui->Act3DFit, &QAction::triggered, easyPlot, &easyPlotWidget::slotFitView3D);

	connect(ui->ActRectPick, &QAction::triggered, easyPlot, &easyPlotWidget::slotRectPickEnable);

	connect(ui->ActCornerRect, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::rect, (int)XRectItem::RectType::diagonal_rect, easyPlot, flag); });
	connect(ui->ActCenterRect, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::rect, (int)XRectItem::RectType::center_rect, easyPlot, flag); });

	connect(ui->ActLine, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::line, -1,easyPlot, flag); });
	
	connect(ui->ActshowGrid, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowGrid2D);
	connect(ui->ActshowGrid3d, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowGrid3D);
	connect(ui->ActShowAxis, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowAxis3D);
	connect(ui->Actcurve, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddLine2D);
	connect(ui->Actchart, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddChart);
	connect(ui->ActBar, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddBar);
	connect(ui->ActFontBmp, &QAction::triggered, easyPlot, &easyPlotWidget::slotGenerateFontTextures);
	connect(ui->ActFontSdf, &QAction::triggered, easyPlot, &easyPlotWidget::slotGenerateFontSdf);
	connect(ui->ActText, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddText);


	auto btn = new QToolButton(this);
	ui->toolBar_3->addWidget(btn);

	btn->setIcon(QIcon(":/icon/rect.svg"));

	QMenu* menu = new QMenu(this);
	menu->addAction(ui->ActCornerRect);
	menu->addAction(ui->ActCenterRect);

	btn->setMenu(menu);
	btn->setPopupMode(QToolButton::MenuButtonPopup);

	QActionGroup *actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui->ActCornerRect);
	actionGroup->addAction(ui->ActCenterRect);
	actionGroup->addAction(ui->ActLine);
	actionGroup->addAction(ui->ActCircle);
	actionGroup->addAction(ui->Acttraiangle);
	actionGroup->setExclusive(true);
	actionGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::slotOpenMesh()
{
	auto name = QFileDialog::getOpenFileName(this, "saveMesh", ".", "(*.xobj *.inp)");
	if (!name.isEmpty()) {
		easyPlot->slotOpenFile(name);
	}
}

void MainWindow::slotSaveMesh()
{
	auto name = QFileDialog::getSaveFileName(this,"saveMesh",".","*.xobj");
	if (!name.isEmpty()) {
		easyPlot->slotSaveFile(name);
	}
}