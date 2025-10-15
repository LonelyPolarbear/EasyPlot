#pragma once

#include <QMainWindow>
#include <QContextMenuEvent>
namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent);
	~MainWindow();

	void contextMenuEvent(QContextMenuEvent* event)override;
public slots:
	void slotSaveMesh();
	void slotOpenMesh();

private:
	Ui::MainWindow* ui;
};
