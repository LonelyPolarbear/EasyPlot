#ifndef FBOTEST_H
#define FBOTEST_H

#include <QDialog>
#include "XPanleApi.h"
namespace Ui {
class FboTest;
}

class lib09_XPANEL_API FboTest : public QDialog
{
    Q_OBJECT

public:
    explicit FboTest(QWidget *parent = nullptr);
    ~FboTest();
public slots:
	void slotBtnOk();
	void slotBtnCancel();
 public:
    void setColorImage(QImage img);
    void setDepthImage(QImage img);

signals:
	void sigTest(int fboWidth, int fboHeight, bool depthEnable, bool multisampleEnable,  int samples, int AttachMode);
    //AttachMode 1 仅颜色附件 2颜色+深度附件 3颜色+深度+模板附件
private:
    Ui::FboTest *ui;
};

#endif // FBOTEST_H
