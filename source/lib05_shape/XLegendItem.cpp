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
	rowLegend->setVisible(true);
	rowLegend->setChecked(true);
	rowLegend->setCurve(curve);
	m_internal->mLegends.push_back(rowLegend);
	addChildItem(rowLegend);
	return true;
}

void XLegendItem::draw(const Eigen::Matrix4f& m)
{
	return XRectItem::draw(m);
}

double XLegendItem::getWidth() const
{
	double maxWidth = 0;
	double maxHeight = 0;
	int rowNum = m_internal->mLegends.size();
	for (auto row : m_internal->mLegends) {
		maxWidth = std::max(maxWidth, row->getWidth());
		maxHeight += row->getHeight();
	}
	return maxWidth;
}

double XLegendItem::getHeight() const
{
	double maxWidth = 0;
	double maxHeight = 0;
	int rowNum = m_internal->mLegends.size();
	for (auto row : m_internal->mLegends) {
		maxWidth = std::max(maxWidth, row->getWidth());
		maxHeight += row->getHeight();
	}
	return maxHeight;
}

void XLegendItem::updateChildPosition(const Eigen::Matrix4f& m)
{
	//设置每一个图例项的位置
	auto selfTransform = this->getTransform();
	auto scenFrameInVirtual = getSceneMatrix();

	double maxWidth = 0;
	//double maxHeight = 0;
	int rowNum = m_internal->mLegends.size();
	for (auto row : m_internal->mLegends) {
		maxWidth = std::max(maxWidth, row->getWidth());
		//maxHeight += row->getHeight();
	}

	double y = 0;
	double step = 2./rowNum;
	{
		LocalCoordCompute localCoord(scenFrameInVirtual * m* selfTransform.matrix());
		auto sx = localCoord.scalex;
		auto sy = localCoord.scaley;
		

		for (int i = 0; i < rowNum; i++) {
			y = 1- (i*step+0.5*step);
			auto totalWidth = m_internal->mLegends[i]->getWidth();
			auto totalHeight = m_internal->mLegends[i]->getHeight();

			auto this_sx = maxWidth * 0.5 / sx;
			auto this_sy = totalHeight * 0.5 / sy;
			m_internal->mLegends[i]->resetTransform();
			m_internal->mLegends[i]->translate(0, y);
			m_internal->mLegends[i]->scale(this_sx, this_sy);
		}
	}
}

