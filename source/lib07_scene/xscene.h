#pragma once
#include "xsceneApi.h"
#include < dataBase/dataobject.h>
#include "lib04_opengl/XOpenGLFramebufferObject.h"
#include "lib06_select/xviewselection.h"
#include "lib06_select/xviewselection2d.h"
#include "lib00_utilty/myUtilty.h"
#include<Eigen/Eigen>
#include <dataBase/XDataArray.h>
class XShape;
class xcamera;
class xshader;
class xShaderManger;
class XGraphicsItem;
class XOpenGLContext;

//Xscne输入的一切坐标都是屏幕坐标系下的值，其中坐标原点在左下角，向右为x轴正方向，向上为y轴正方向。
class LIB07_SCENE_API XScene :public DataBaseObject {
protected:
	XScene();
	 ~XScene();

	 public:
		 bool addShape(std::shared_ptr<XShape> shape);
		 bool addGraphicsItem(std::shared_ptr<XGraphicsItem> shape);
		 bool removeShape(std::shared_ptr<XShape> shape);
		 bool removeGraphicsItem(std::shared_ptr<XGraphicsItem> shape);
		 void removeAll();
		 std::shared_ptr<XShape> getShape(uint64_t id);
		 std::shared_ptr<XGraphicsItem> getGraphicsItem(uint64_t id);

		void setCamera(std::shared_ptr<xcamera> camera);
		std::shared_ptr<xcamera> getCamera();

		void setContext(std::shared_ptr<XOpenGLContext> context);
		std::shared_ptr<XOpenGLContext> getContext();

		void setShaderManger(std::shared_ptr<xShaderManger> shader);
		std::shared_ptr<xShaderManger> getShaderManger();

		void setViewSelection(std::shared_ptr<XViewSelection> viewselection);
		std::shared_ptr<XViewSelection> getViewSelection();

		void setViewport(int x, int y, int w, int h);

		int getViewportWidth() const;

		int getViewportHeight() const;

		void updateViewport();

		std::vector<XViewSelection::SelectData> getPointSelection(int x, int y);

		std::vector<XViewSelection2D::SelectData> getPointSelection2D(int x, int y);

		std::vector<std::vector<XViewSelection::SelectData>> getBoxSelection(int x, int y, int w, int h);
		std::vector<std::vector<XViewSelection2D::SelectData>> getBoxSelection2D(int x, int y, int w, int h);

		void updateUbo2GPU();

		void createUbo();
		//交互
		void  render();
		
		//仅仅是为了测试离屏渲染
		sptr<XUCharArray2D> renderFbo();

		void virtual render3D();

		void virtual render2D();

		void updateVirtualWorldFrame();

		void setMousePos(int x, int y);

		void makeCurrent();

		void swapBuffers();

		void doneCurrent();

		virtual void resizeEvent(int x, int y, int w, int h);

		virtual void mousePressEvent(int x, int y, int type);

		virtual void mouseDoublePressEvent(int x, int y, int type);

		virtual void mouseMoveEvent(int x, int y);

		virtual void mouseReleaseEvent(int x, int y, int type);

		virtual void wheelEvent(int angle,int x,int y);

		void fitView2D();

		void setGrid2dVisible(bool visible);

		bool isGrid2dVisible();

		void setGrid3dVisible(bool visible);

		bool isGrid3dVisible();

		void setAxis3dVisible(bool visible);

		void fitView3D();

		void rotate(int x1, int y1, int x2, int y2);		//对相机的操作

		void rotate(myUtilty::Vec2u curpos, myUtilty::Vec2u lastpos);		//对相机的操作

		void translate(int x1, int y1, int x2, int y2);		//对相机的操作

		void translate(myUtilty::Vec2u curpos,myUtilty::Vec2u lastpos );		//对相机的操作

		//屏幕到视口坐标系的转换
		myUtilty::Vec2i screenPos2ViewportPos(myUtilty::Vec2u) const;

		//视口到屏幕坐标系的转换
		myUtilty::Vec2u viewportPos2ScreenPos(myUtilty::Vec2i) const;

		//场景坐标系到视口坐标系的转换
		myUtilty::Vec2i scenePos2ViewportPos(myUtilty::Vec2f) const;

		//视口坐标系到场景坐标系的转换
		myUtilty::Vec2f viewportPos2ScenePos(myUtilty::Vec2i) const;

		//屏幕坐标系到场景坐标系的转换
		myUtilty::Vec2f screenPos2ScenePos(myUtilty::Vec2u) const;

		//场景坐标系到屏幕坐标系的转换	
		myUtilty::Vec2u scenePos2ScreenPos(myUtilty::Vec2f) const;

	
		/// <summary>
		/// 屏幕场景坐标系到场景坐标系的转换，原点在d->startx,d->starty,向右为x轴正方向，向上为y轴正方向
		/// </summary>
		/// <returns></returns>
		Eigen::Matrix4f sceneScreenPos2ScenePos();

		static Eigen::Matrix4f sceneScreenPos2ScenePosMat(std::shared_ptr<DataBaseObject> scene);

		//判断点是否在视口内(点是屏幕坐标系下的点，原点左下角 向右为x轴正方向，向上为y轴正方向)
		bool isBelongtoViewPort(int x, int y);

		//获取场景的包围盒
		myUtilty::BoundBox computeBoundBox();

		int mousePosx{0};
		int mousePosy{0};

		std::shared_ptr<XUCharArray2D> grabFramebuffer();

		std::shared_ptr<XUCharArray2D> grabFramebuffer(int startx, int starty, int width, int height,int dest_x, int dest_y, int dest_width, int dest_height);
		
private:
	class Internal;
	std::unique_ptr<Internal> d;
};