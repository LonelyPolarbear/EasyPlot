#pragma once
#include "xselectApi.h"
#include < dataBase/dataobject.h>
#include <set>
#include "lib04_opengl/XOpenGLFramebufferObject.h"
class XShape;
class xcamera;
class xshader;
class LIB06_SELECT_API XViewSelection :public DataBaseObject {
public:
	struct SelectData {
		uint32_t objectId{0};
		uint32_t primitiveId{0};
	};
protected:
	XViewSelection();
	 ~XViewSelection();
public:
void setPickShader(std::shared_ptr<xshader> shader);

bool create();

bool updateBufferSize(int width, int height);

std::shared_ptr< XOpenGLFramebufferObject> getFbo(int index);

SelectData getPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight, int layer = 0);

std::vector<SelectData> getAllPointSelection(int posx, int posy, int ViewportWidth, int ViewportHeight);

std::vector< std::vector<XViewSelection::SelectData>> getBoxSelection(int posx, int posy,int width, int height, int ViewportWidth, int ViewportHeight);

/// <summary>
/// 更新拾取，必须在主线程调用
/// </summary>
/// <param name="objects"></param>
/// <param name="camera"></param>
/// <returns></returns>
 bool update(std::set<std::shared_ptr<XShape>> objects, std::shared_ptr<xcamera> camera);

 bool renderLayer(std::set<std::shared_ptr<XShape>> objects, std::shared_ptr<xcamera> camera,int layer);

 int getPeelLayerNum()const;
private:
	class Internal;
	std::unique_ptr<Internal> d;
};