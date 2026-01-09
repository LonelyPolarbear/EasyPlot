#include <QHBoxLayout> 
#include <QWidget>
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog> 
#include<easyPlot/easyPlotWidget.h>

#include <qtoolbutton.h>
#include "lib05_shape/actor2d/XRectItem.h"
#include <lib09_panel/ColorPickButton.h>
#include <lib09_panel/CustomToolButton.h>

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

	setFocusPolicy(Qt::StrongFocus); // 添加这行
	setMouseTracking(true);
	setContextMenuPolicy(Qt::DefaultContextMenu);

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

	connect(ui->ActCornerRect, &QAction::triggered, [this](bool flag) {
		setDarwType(render::graphicsItemType::rect, (int)XRectItem::RectType::diagonal_rect, easyPlot, flag);
	});
	connect(ui->ActCenterRect, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::rect, (int)XRectItem::RectType::center_rect, easyPlot, flag); });

	connect(ui->ActLine, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::line, -1, easyPlot, flag); });

	connect(ui->ActCircle, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::circle, -1, easyPlot, flag); });

	connect(ui->Acttraiangle, &QAction::triggered, [this](bool flag) {setDarwType(render::graphicsItemType::triangle, -1, easyPlot, flag); });
	
	connect(ui->ActshowGrid, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowGrid2D);
	connect(ui->ActshowGrid3d, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowGrid3D);
	connect(ui->ActShowAxis, &QAction::triggered, easyPlot, &easyPlotWidget::slotShowAxis3D);
	//connect(ui->Actcurve, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddLine2D);
	connect(ui->Actchart, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddChart);
	connect(ui->ActBar, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddBar);
	connect(ui->ActFontBmp, &QAction::triggered, easyPlot, &easyPlotWidget::slotGenerateFontTextures);
	connect(ui->ActFontSdf, &QAction::triggered, easyPlot, &easyPlotWidget::slotGenerateFontSdf);
	connect(ui->ActText, &QAction::triggered, easyPlot, &easyPlotWidget::slotAddText);

	connect(ui->ActScreenTipVisible, &QAction::triggered, easyPlot, &easyPlotWidget::slotScreenTextVisible);
	connect(ui->ActScreenShot, &QAction::triggered, easyPlot, &easyPlotWidget::slotScreenShot);
	connect(ui->Actaxis2d, &QAction::triggered, easyPlot, &easyPlotWidget::slotAxis2D);
	connect(ui->ActfboTest, &QAction::triggered, easyPlot, &easyPlotWidget::slotFboTest);

	connect(ui->ActWaveSin, &QAction::triggered, [&]() {easyPlot->slotAddLine2D(1); });
	connect(ui->ActWaveTri, &QAction::triggered, [&]() {easyPlot->slotAddLine2D(2); });
	connect(ui->ActWaveSquare, &QAction::triggered, [&]() {easyPlot->slotAddLine2D(3); });

	connect(ui->ActComputeTest, &QAction::triggered, [&]() {easyPlot->slotComputeShaderTest(); });

	QActionGroup *actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui->ActCornerRect);
	actionGroup->addAction(ui->ActCenterRect);
	actionGroup->addAction(ui->ActLine);
	actionGroup->addAction(ui->ActCircle);
	actionGroup->addAction(ui->Acttraiangle);
	actionGroup->setExclusive(true);
	actionGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

	{
		auto btn = new CustomToolButton(this);
		btn->setCheckable(true);
		ui->toolBar_3->addWidget(btn);

		btn->setIcon(QIcon(":/icon/rect.svg"));

		QMenu* menu = new QMenu(this);
		menu->setStyleSheet(R"(
		/* 强制显示勾选标记图标 */
		QMenu::item:checked {
			background-color: #e0e0e0;
			/* 手动指定勾选图标（如果系统默认不显示） */
			background-image: url(:/icons/check.png); /* 可自行添加一个√图标 */
			background-repeat: no-repeat;
			background-position: left center; /* 图标左对齐 */
			background-origin: content-box;
		}
		 )");
		menu->addAction(ui->ActCornerRect);
		menu->addAction(ui->ActCenterRect);


		connect(actionGroup, &QActionGroup::triggered, this, [=](QAction* action) {
			if (action->isChecked()) {
				if (action == ui->ActCornerRect || action == ui->ActCenterRect)
					btn->setChecked(true);
				else
					btn->setChecked(false);
			}
			else {
				btn->setChecked(false);
			}
			});


		btn->setMenu(menu);
		btn->setPopupMode(QToolButton::MenuButtonPopup);

	}
	{
	
		auto btn = new CustomToolButton(this);
		btn->setCheckable(false);
		ui->toolBar_4->addWidget(btn);

		btn->setIcon(QIcon(":/icon/curve.svg"));

		QMenu* menu = new QMenu(this);
		menu->setStyleSheet(R"(
		/* 强制显示勾选标记图标 */
		QMenu::item:checked {
			background-color: #e0e0e0;
			/* 手动指定勾选图标（如果系统默认不显示） */
			background-image: url(:/icons/check.png); /* 可自行添加一个√图标 */
			background-repeat: no-repeat;
			background-position: left center; /* 图标左对齐 */
			background-origin: content-box;
		}
		 )");
		menu->addAction(ui->ActWaveSin);
		menu->addAction(ui->ActWaveSquare);
		menu->addAction(ui->ActWaveTri);

		btn->setMenu(menu);
		btn->setPopupMode(QToolButton::MenuButtonPopup);
	}
	
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{

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