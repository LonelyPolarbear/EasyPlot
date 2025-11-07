#include "GridSetDlg.h"
#include "ui_GridSetDlg.h"

GridSetDlg::GridSetDlg(const GridSetParam& parm, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::GridSetDlg)
{
    ui->setupUi(this);

   setGridSetParam(parm);

    qRegisterMetaType<GridSetParam>("GridSetParam");

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotBtnCancel()));
}

GridSetDlg::~GridSetDlg()
{
    delete ui;
}

void GridSetDlg::setGridSetParam(const GridSetParam& parm)
{
	ui->gridNum->setValue(parm.gridNum);
	ui->gridSpace->setValue(parm.gridSpace);
	ui->xrange_min->setValue(parm.rangex_min);
	ui->xrange_max->setValue(parm.rangex_max);
	ui->yrange_min->setValue(parm.rangey_min);
	ui->yrange_max->setValue(parm.rangey_max);
}

void GridSetDlg::slotBtnOk()
{
    GridSetParam parm;
    parm.gridNum = ui->gridNum->value();
    parm.gridSpace = ui->gridSpace->value();
    parm.rangex_min = ui->xrange_min->value();
    parm.rangex_max = ui->xrange_max->value();
    parm.rangey_min = ui->yrange_min->value();
    parm.rangey_max = ui->yrange_max->value();

    emit sigGridSetParam(parm);
    mParm = parm;
    //accept();
}

void GridSetDlg::slotBtnCancel()
{
    close();
}
