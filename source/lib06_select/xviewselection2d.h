#pragma once
#include "xselectApi.h"
#include < dataBase/XDataBaseObject.h>
#include <set>
#include "lib04_opengl/XOpenGLFramebufferObject.h"
class XGraphicsItem;
class xcamera;
class xshader;
#include <Eigen/Eigen>

class LIB06_SELECT_API XViewSelection2D :public XDataBaseObject {
public:
	struct XSelectData {
		uint32_t objectId{0};
		uint32_t primitiveId{0};
	};
protected:
	XViewSelection2D();
	 ~XViewSelection2D();
public:
void setPickShader(std::shared_ptr<xshader> shader);

void setPickFillShader(std::shared_ptr<xshader> shader);

void setScene(std::shared_ptr<XDataBaseObject> scene);

void setGetMatrixforScreen2Scene(std::function<Eigen::Matrix4f(std::shared_ptr<XDataBaseObject>)> fn);

bool create();

bool updateBufferSize(int width, int height);

std::shared_ptr< XOpenGLFramebufferObject> getFbo(int index);

XSelectData getPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight, int layer = 0);

std::vector<XSelectData> getAllPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight);

std::vector< std::vector<XViewSelection2D::XSelectData>> getBoxSelection(int posx, int posy,int width, int height, int ViewportWidth, int ViewportHeight);

/// <summary>
/// 更新拾取，必须在主线程调用
/// </summary>
/// <param name="objects"></param>
/// <param name="camera"></param>
/// <returns></returns>
 int update(std::set<std::shared_ptr<XGraphicsItem>> objects, std::shared_ptr<xcamera> camera, const Eigen::Matrix4f& m);

 bool renderLayer(std::set<std::shared_ptr<XGraphicsItem>> objects, std::shared_ptr<xcamera> camera,int layer, const Eigen::Matrix4f& m);

 int getPeelLayerNum()const;
private:
	class Internal;
	std::unique_ptr<Internal> d;
};