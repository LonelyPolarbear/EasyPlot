#ifndef FONTSETDLG_H
#define FONTSETDLG_H

#include <QDialog>
#include <lib00_utilty/XUtilty.h>
#include "XPanleApi.h"

namespace Ui {
class FontSetDlg;
}

class lib09_XPANEL_API FontSetDlg : public QDialog
{
    Q_OBJECT
    public:
     struct TextSetInfo {
        QString content ="请输入文字！";
        int alignH =-1; //-1 0 1 对应左中右
        int alignV =0; //-1 0 1 对应上中下
        double x =0;
        double y =0;
        bool isFixed =false;
        double fixWidth =0;
        XQ::Vec4f color =XQ::Vec4f(1,1,11);
        int tackMode=0;  //local_complete,	local_center,sceneScreen_complete,sceneScreen_center
        int refOrigin =0; //left_top,left_bottom,right_top,right_bottom
    };
public:
    explicit FontSetDlg(const TextSetInfo& info,QWidget *parent = nullptr);
    ~FontSetDlg();

    void addFun(std::function<void()> callback);
public slots:
    void slotBtnOk();
    void slotBtnCancel();
signals:
     void sigSetTextInfo();
 public:
     TextSetInfo getTextInfo();
     void setTexInfo(const TextSetInfo& info);
private:
    Ui::FontSetDlg *ui;
    TextSetInfo mTextInfo;
    std::function<void()> m_callback;
};

#endif // FONTSETDLG_H
