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

    Q_INVOKABLE void SlotAddLog(const std::string& log,int level);
private:
    Ui::XMainWindow *ui;
protected:
    class Internal;
    std::shared_ptr<Internal> mData;
};

#endif // XMAINWINDOW_H
