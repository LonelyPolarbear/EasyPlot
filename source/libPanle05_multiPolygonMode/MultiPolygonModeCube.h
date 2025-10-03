#pragma once
#ifdef  LIBPANEL05_MULTIPOLYGONMODE_CUBE_DLL	
#define lib05_openglwodget_API __declspec(dllexport)
#else
#define lib05_openglwodget_API __declspec(dllimport)
#endif 

#include <libPanle01_XOpenGLWidget/XOpenGLWidget.h>

class QOpenGLContext;
class QWindow;
class QMouseEvent;
class QWheelEvent;

/// <summary>
///
/// </summary>
class lib05_openglwodget_API MultiPolygonModeCube : public XOpenGLWidget
{
	Q_OBJECT
public:
	explicit MultiPolygonModeCube(QWidget* parent);
	~MultiPolygonModeCube();

	void virtual render() override;
	void virtual initGLResource();

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	struct Internal;
	std::unique_ptr<Internal> d;
};
