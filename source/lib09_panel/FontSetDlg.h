#ifndef FONTSETDLG_H
#define FONTSETDLG_H

#include <QDialog>

namespace Ui {
class FontSetDlg;
}

class FontSetDlg : public QDialog
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
