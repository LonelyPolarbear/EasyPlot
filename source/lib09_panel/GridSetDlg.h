#ifndef GRIDSETDLG_H
#define GRIDSETDLG_H

#include "XPanleApi.h"

#include <QDialog>

namespace Ui {
class GridSetDlg;
}

struct lib09_XPANEL_API GridSetParam
{
    int gridNum;
    int gridSpace;
    double rangex_min;
    double rangex_max;
    double rangey_min;
    double rangey_max;
};
class lib09_XPANEL_API GridSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GridSetDlg(const GridSetParam& parm,QWidget *parent = nullptr);
    ~GridSetDlg();
    public:
    void setGridSetParam(const GridSetParam& parm);
public slots:
	void slotBtnOk();
	void slotBtnCancel();
signals:
	void sigGridSetParam(const GridSetParam& parm);
private:
    Ui::GridSetDlg *ui;
    GridSetParam mParm;
};

#endif // GRIDSETDLG_H
