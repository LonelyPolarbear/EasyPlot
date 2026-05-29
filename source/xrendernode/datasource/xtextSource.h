#pragma once
#include "xshapeSource.h"

enum class HAlign {
	Left,
	Center,
	Right
};
enum class VAlign {
	Top,
	Middle,
	Bottom
};

class XRENDERNODE_API XTextSource :public XShapeSource {
protected:
	XTextSource();
	virtual ~XTextSource();
public:

	void updateVertextCoordArray() override;

	void updateFaceIndexArray()override;

	void updateFaceColorArray()override {};

	void updateVertextNormalArray()override {};

	void updateVertexColorArray()override{};

	void updateLineIndexArray() override{};

	void updateLineColorArray() override{};

	void updateVertexIndexArray() override{};

	void updateInstancedArray() override{}

	virtual void updateCustomArray() override;

	void setHasUpdated() override;

	void updateTexCoord();

	void setText(const std::wstring& text);
	void setFontSize(double fontSize);
	void setFixedWidth(double width);
	void setIsFixed(bool isFixed);

	std::wstring getText() const;
	double getFontSize() const;
	double getFixedWidth() const;
	bool isFixedWidth() const;
protected:
	class Internal;
	std::unique_ptr<Internal> mData;
};