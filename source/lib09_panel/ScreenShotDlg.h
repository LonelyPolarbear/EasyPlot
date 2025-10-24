#ifndef SCREENSHOTDLG_H
#define SCREENSHOTDLG_H

#include <QDialog>
#include "XPanleApi.h"

namespace Ui {
class ScreenShotDlg;
}

class lib09_XPANEL_API ScreenShotDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenShotDlg(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, QWidget *parent = nullptr);
    ~ScreenShotDlg();

public slots:
	void slotBtnOk();
	void slotBtnCancel();

    signals:
	void sigScreenShot(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h,QString fileName);
private:
    Ui::ScreenShotDlg *ui;
};

#endif // SCREENSHOTDLG_H
