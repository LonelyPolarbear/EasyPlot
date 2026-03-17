#ifndef XMAINWINDOW_H
#define XMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class XMainWindow;
}

class XMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit XMainWindow(QWidget *parent = nullptr);
    ~XMainWindow();

    void BuildUI();
    void InitLeftTab();
    void InitRightTab();
    void InitBotTab();

    void InitView();
private:
    Ui::XMainWindow *ui;
protected:
    class Internal;
    std::shared_ptr<Internal> mData;
};

#endif // XMAINWINDOW_H
