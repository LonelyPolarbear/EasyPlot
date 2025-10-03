#pragma once

#include <QMainWindow>
namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent);
	~MainWindow();
public slots:
	void slotSaveMesh();
	void slotOpenMesh();

private:
	Ui::MainWindow* ui;
};
