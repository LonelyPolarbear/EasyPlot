#include "XLegendItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib08_freetype/xfreetype.h>
#include "XTextItem.h"
#include "XRectItem.h"
#include "XCheckBoxItem.h"
#include "XLegendRowItem.h"
class XLegendItem::Internal {
public:
	Internal() {
		
	}
	std::vector<std::shared_ptr< XLegendRowItem>> mLegends;
};

XLegendItem::XLegendItem(std::shared_ptr<XGraphicsItem> parent):XRectItem(parent), m_internal(new Internal)
{
	
}

XLegendItem::~XLegendItem()
{
	
}

bool XLegendItem::addCurve(std::shared_ptr<XGraphicsItem> curve)
{
	auto rowLegend =makeShareDbObject<XLegendRowItem>();
	rowLegend->setCurve(curve);
	m_internal->mLegends.push_back(rowLegend);
	addChildItem(rowLegend);
	return true;
}

void XLegendItem::updateChildPosition(const Eigen::Matrix4f& m)
{
}

