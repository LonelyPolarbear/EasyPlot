#pragma once
#ifdef  LIB_EASYPLOT_DLL	
#define lib06_easyplot_API __declspec(dllexport)
#else
#define lib06_easyplot_API __declspec(dllimport)
#endif 

#include <XOpenGLWidget/XOpenGLWidget.h>
#include <lib00_utilty/XUtilty.h>
#include <xrendernode/XRenderNode.h>
#include <dataBase/XDataArray.h>

class QOpenGLContext;
class QWindow;
class QMouseEvent;
class QWheelEvent;
class QTimerEvent;
class QContextMenuEvent;


class XGraphicsItem;
struct DarwItemData {
	std::shared_ptr<XGraphicsItem> item;
	graphicsItemType type;
	std::shared_ptr< XFloatArray> coordArray;
};


/// <summary>
///
/// </summary>
class lib06_easyplot_API easyPlotWidget : public XOpenGLWidget
{
	Q_OBJECT
public:
	explicit easyPlotWidget(QWidget* parent);
	~easyPlotWidget();

	void virtual render() override;

	void virtual initGLResource();

	void print();

	void buildUI();

	void attachSignalSlot();

	uint32_t getSelectId();

	bool isAllowContextmenu() const;

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	virtual void contextMenuEvent(QContextMenuEvent* event)override;
	void timerOut();
	DarwItemData createItem(graphicsItemType type);
public slots:
	void slotSaveFile(const QString& fileName);
	void slotOpenFile(const QString& fileName);
	void slotDelete();
	void slotDeleteAll();
	void slotVisibleAll();
	void slotInvisible();
	void slotCreateCube();
	void slotCreateSphere();
	void slotCreateCone();
	void slotSetting();

	void slotFileLoadFinished();
	void slotAnyTaskFinished();
	void slotRectPickEnable(bool flag);

	//2D
	void slotSetDarwItemType(int type,int method);
	void slotFitView2D();
	void slotFitView3D();
	void updateItem();

	void slotShowGrid2D(bool flag);
	void slotShowGrid3D(bool flag);
	void slotShowAxis3D(bool flag);

	void slotAddLine2D(int curveType);

	void slotAddChart();

	void slotAddBar();

	void slotAddText();

	void slotGenerateFontTextures();

	void slotGenerateFontSdf();

	void slotScreenTextVisible(bool flag);

	//½ØÍ¼
	void slotScreenShot();

	void slotAxis2D();

	void slotFboTest();

	//¼ÆËã×ÅÉ«Æ÷²âÊÔ
	void slotComputeShaderTest();

private:
	struct Internal;
	std::unique_ptr<Internal> d;
};
