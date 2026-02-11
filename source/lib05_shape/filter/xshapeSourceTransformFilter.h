#pragma once
#include "../xshapeApi.h"
#include "../datasource/xshapeSource.h"
#include <lib00_utilty/XUtilty.h>
#include <Eigen/Eigen>


class LIB05_SHAPE_API XShapeSourceTransformFilter : public XShapeSource {
protected:
	XShapeSourceTransformFilter();

	~XShapeSourceTransformFilter();
public:
	void translate(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setScale(float x, float y, float z);
	void rotate(float angle, XQ::Vec3f dir);

	XQ::Vec3f getPosition() const;
	XQ::Vec3f getScale() const;

	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	void scale(float x, float y, float z);

	void setTransform(const Eigen::Affine3f& trans);

	Eigen::Affine3f getTransform() const;
	
	void setInput(sptr<XShapeSource> input);

	bool update() override;

	virtual void updateVertextCoordArray(){};

	virtual void updateVertextNormalArray(){};

	virtual void updateVertexColorArray(){};

	virtual void updateFaceIndexArray(){};

	virtual void updateFaceColorArray(){};

	virtual void updateLineIndexArray(){};

	virtual void updateLineColorArray(){};

	virtual void updateVertexIndexArray(){};
protected:
	bool updateSelf();
	class Internal;
	std::shared_ptr<Internal> mData;
};