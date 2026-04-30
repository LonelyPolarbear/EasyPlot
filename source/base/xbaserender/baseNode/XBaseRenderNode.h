#include "base/xbaserender/xbaserenderApi.h"
#include <dataBase/XDataBaseObject.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <xsignal/XSignal.h>
#include <lib00_utilty/XUtilty.h>
/**
 * @class XBaseRenderNode
 * @brief 基础渲染节点
 */
class xShaderManger;
class XBaseRender;
class XBASERENDER_API XBaseRenderNode : public XDataObject {
	REGISTER_CLASS_META_DATA(XBaseRenderNode, XDataObject);
protected:
	XBaseRenderNode();
	virtual ~XBaseRenderNode();
public:
		virtual void draw(sptr<XBaseRender>  render,const Eigen::Matrix4f& parentMatrix, bool isNormal) = 0;

		virtual void drawInstance(sptr<XBaseRender>  render, const Eigen::Matrix4f& parentMatrix) = 0;

		virtual bool addChildRenderNode(sptr<XBaseRenderNode> child) =0;

		virtual bool removeChildRenderNode(sptr< XBaseRenderNode> child) =0;

		virtual void clearChildrenRenderNode() =0;

		virtual int getChildRenderNodeCount() const =0;

		virtual sptr<XBaseRenderNode> getChildRenderNode(int index) const =0;

		virtual sptr<XBaseRenderNode> getRenderNodeParent() const =0;

		virtual void setVisible(bool visible) =0;

		virtual bool isVisible() =0;

		virtual void setShaderManger(std::shared_ptr<xShaderManger> shaderManger)=0;

		virtual std::shared_ptr<xShaderManger> getShaderManger() const=0;

		virtual XQ::BoundBox getBoundBox(const Eigen::Matrix4f& m) const = 0;

		virtual XQ::BoundBox getThisBoundBox(const Eigen::Matrix4f& m) const = 0;

		virtual Eigen::Affine3f getTransform() const = 0;

		virtual void getChainTransform(Eigen::Affine3f& afiine) const = 0;

		virtual void setTransform(const Eigen::Affine3f& transform) = 0;

		virtual void resetTransform() = 0;

		virtual void translate(float x, float y, float z) = 0;

		virtual void setPosition(float x, float y, float z) = 0;

		virtual void setScale(float x, float y, float z) = 0;

		virtual void rotate(float angle, XQ::Vec3f dir) = 0;

		/**
		 * @brief 旋转变换 angle角度，不是弧度
		 */
		virtual void rotateX(float angle) = 0;
		virtual void rotateY(float angle) = 0;
		virtual void rotateZ(float angle) = 0;
		virtual void scale(float x, float y, float z) = 0;

		virtual int64_t getID() const =0;
};