#include "XChartItem2.h"
#include "XGridItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include "xrendernode/actor2d/XPolyline.h"
#include <Eigen/Eigen>

#include <lib04_opengl/XOpenGLEnable.h>
#include "xrendernode/actor2d/XTextItem.h"
#include "xrendernode/actor2d/XAxisItem.h"
#include "xrendernode/actor2d/XLineItem.h"
#include "xrendernode/actor2d/XLegendItem.h"
#include <Eigen/Eigen>


class XChartItem2::Internal {
public:
	std::vector<std::shared_ptr<XGraphicsItem>> m_polylines;	//线段集合
	std::shared_ptr< XGridItem> m_gridItem;	//网格
	std::shared_ptr< XLegendItem> m_legend = makeShareDbObject<XLegendItem>();	//图例

	sptr<XAxisItem> m_axisx;				//X轴
	sptr<XAxisItem> m_axisy;				//Y轴
	sptr<XTextItem> m_title;				//标题

	int m_xlabelNum =11;					//X轴标签数量
	int m_ylabelNum =4;						//Y轴标签数量
	bool m_isShowGrid = true;			//是否显示网格

	XQ::Vec2f mXRange = XQ::Vec2f(-1, 1);

	XQ::Vec2f mYRange = XQ::Vec2f(-1, 1);
	
	Internal() {
		//创建默认的图元
		{
			m_title = makeShareDbObject<XTextItem>(nullptr);
			m_title->setHAlignment(XTextItem::HAlign::Center);
			m_title->setVAlignment(XTextItem::VAlign::Middle);
			m_title->setPositionType(XGL::PositionType::local_center);
			m_title->setSingleColor(XQ::Vec4f(1, 1, 0, 1));
			m_title->setText(L"标题测试");
			m_title->setFontSize(20);
		}
		{
			m_axisy = makeShareDbObject<XAxisItem>();
			m_axisy->setLayout(XGL::Layout::vertical);
			m_axisy->updateTextPos();

			m_axisx = makeShareDbObject<XAxisItem>();
			m_axisx->setLayout(XGL::Layout::horizontal);
			m_axisx->updateTextPos();

			m_axisx->getLine()->setSingleColor(XQ::Vec4f(1, 0, 0, 1));
			m_axisy->getLine()->setSingleColor(XQ::Vec4f(0, 1, 0, 1));

			m_axisx->getLine()->setFixedLine(true);
			m_axisy->getLine()->setFixedLine(true);

			m_axisx->getLine()->setLineWidth(3);
			m_axisy->getLine()->setLineWidth(3);

			m_axisx->setRange(mXRange.x(), mXRange.y());
			m_axisy->setRange(mYRange.x(), mYRange.y());
		}
		{
			m_gridItem =makeShareDbObject<XGridItem>();
			m_gridItem->setIsScreenGrid(false);
			m_gridItem->setShowAxis(false);
		}
	}
};

XChartItem2::XChartItem2(std::shared_ptr<XGraphicsItem> parent):XGraphicsItem(parent),d(new Internal())
{
	this->setDrawType(PrimitveType::line_strip_adjacency);
	auto coord = makeShareDbObject<XFloatArray>();

	coord->setComponent(3);
	coord->setNumOfTuple(4);

	coord->setTuple(0, -1, -1, 0);
	coord->setTuple(1, 1, -1, 0);
	coord->setTuple(2, 1, 1, 0);
	coord->setTuple(3, -1, 1, 0);

	this->setCoordArray(coord);

	auto index = makeShareDbObject<XUIntArray>();
	index->setComponent(3);
	index->setNumOfTuple(2);
	index->setTuple(0, 1, 2, 3);
	index->setTuple(1, 1, 3, 4);

	this->setIndexArray(index);

	this->setLineWidth(2);

	this->setIsFilled(false);

	this->setBackgroundColor(XQ::Vec4f(1.f, 1.f, 0.f, 1.0f));

	m_isShowGrid = true;
	addChildItem(d->m_gridItem);
	addChildItem(d->m_legend);
	addChildItem(d->m_title);
	addChildItem(d->m_axisx);
	addChildItem(d->m_axisy);
	updateGridFrame();
}

XChartItem2::~XChartItem2()
{
}

void XChartItem2::setRect(float x, float y, float w, float h)
{
	m_coordArray->setTuple(0, x,y, 0);
	m_coordArray->setTuple(1, x+w,y, 0);
	m_coordArray->setTuple(2, x+w,y+h, 0);
	m_coordArray->setTuple(3, x,y+h, 0);
	m_coordArray->Modified();
}

void XChartItem2::addPolyline(std::shared_ptr<XGraphicsItem> polyline)
{
	if (polyline) {
		if (!polyline->getShaderManger()) {
			polyline->setShaderManger(this->getShaderManger());
		}
		d->m_polylines.push_back(polyline);
		d->m_legend->addCurve(polyline);
		d->m_gridItem->addLine(polyline);
	}
}

void XChartItem2::clearPolylines()
{
	d->m_polylines.clear();
}

void XChartItem2::draw(const Eigen::Matrix4f& m)
{
	initiallize();
	{
		d->m_axisx->updateTextPos();
		d->m_axisy->updateTextPos();

		/*auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);

		if (!d->m_gridItem->getShaderManger()) {
			d->m_gridItem->setShaderManger(getShaderManger());
		}
		d->m_gridItem->draw(m * getTransform().matrix());
		glEnableObj->restore();*/

		XGraphicsItem::draw(m);
#if 0
		auto selfTransform = this->getTransform();
		auto clipTransform = selfTransform * d->m_gridItem->getTransform() * selfTransform.inverse();
		beginClip(m*clipTransform.matrix());

		Eigen::Matrix4f lineParentTransform = m * selfTransform.matrix() *d->m_gridItem->getTransform().matrix()* d->m_gridItem->getGridTransform().matrix();
		for (auto polyline : d->m_polylines) {
			polyline->draw(lineParentTransform);
		}

		endClip();
#endif
	}
}

void XChartItem2::pickBorderDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickBorderDraw(shader, m);
}

void XChartItem2::pickFillDraw(std::shared_ptr<xshader> shader, const Eigen::Matrix4f& m)
{
	return XGraphicsItem::pickFillDraw(shader, m);
}

void XChartItem2::fitView()
{
	for(auto line : d->m_polylines){
		auto box = line->getBoundBox();
		d->mXRange.x() = std::min<float>(d->mXRange.x(), box.xmin);
		d->mXRange.y() = std::max<float>(d->mXRange.y(), box.xmax);

		d->mYRange.x() = std::min<float>(d->mYRange.x(), box.ymin);
		d->mYRange.y() = std::max<float>(d->mYRange.y(), box.ymax);
	}
}

void XChartItem2::setBackgroundColor(const XQ::Vec4f& color)
{
	m_fillColor = color;
}

void XChartItem2::setTtitle(const std::wstring& title)
{
	d->m_title->setText(title);
}

void XChartItem2::chartTranslate(const XQ::Vec2f& lastPos_, const XQ::Vec2f& curPos_)
{
	auto parentTran = getParentAccumulateTransform();
	
	//表达的是网格坐标中的点到场景坐标系的变换
	auto ssss = d->m_gridItem->getGridTransform();
	auto chartTransform =getTransform()*d->m_gridItem->getTransform()*d->m_gridItem->getGridTransform();

	Eigen::Affine3f scene2Chart = chartTransform.inverse();
	Eigen::Vector3f curPos = scene2Chart * Eigen::Vector3f(curPos_.x(), curPos_.y(), 0);
	Eigen::Vector3f lasPos = scene2Chart * Eigen::Vector3f(lastPos_.x(), lastPos_.y(), 0);

	auto dx = curPos.x() - lasPos.x();
	auto dy = curPos.y() - lasPos.y();

	auto range =d->m_axisx->getRange();
	range.x() -= dx;
	range.y() -= dx;
	d->m_axisx->setRange(range.x(), range.y());

	{
		auto range = d->m_axisy->getRange();
		range.x() -= dy;
		range.y() -= dy;
		d->m_axisy->setRange(range.x(), range.y());
	}
	
	updateGridFrame();
}

void XChartItem2::chartSale(float dx, float dy)
{
	{
		dx = dx > 1 ? 0.1 : -0.1;
		dy = dy > 1 ? 0.1 : -0.1;

		auto range = d->m_axisx->getRange();
		auto len = range.y() - range.x();
		range.x() -= dx * len;
		range.y() += dy * len;
		d->m_axisx->setRange(range.x(), range.y());
	}
	{
		auto range = d->m_axisy->getRange();
		auto len = range.y() - range.x();
		range.x() -= dy * len;
		range.y() += dx * len;
		d->m_axisy->setRange(range.x(), range.y());
	}
	
	updateGridFrame();
}

void XChartItem2::updateGridFrame()
{
	//当轴的范围变化时，需要更新网格坐标系
	auto sx = d->m_axisx->getRange().y() - d->m_axisx->getRange().x();
	auto sy = d->m_axisy->getRange().y() - d->m_axisy->getRange().x();
	d->m_gridItem->gridReset();
	d->m_gridItem->gridSetSale(sx, sy);
	auto origin = XQ::Vec2d(d->m_axisx->getRange().x(), d->m_axisy->getRange().x());
	d->m_gridItem->setOrigin(origin);
}

void XChartItem2::updateChildPosition(const Eigen::Matrix4f& m)
{
	//对于自身的绘制，先更新数据
	auto selfTransform = this->getTransform();
	auto scenFrameInVirtual = getSceneMatrix();

	LocalCoordCompute localCoord(scenFrameInVirtual * m * selfTransform.matrix());

	Eigen::Vector3f leftBot = localCoord.compute(XGL::Orientation::left_bottom, 18, 18);
	Eigen::Vector3f rightBot = localCoord.compute(XGL::Orientation::right_bottom, 18, 18);
	Eigen::Vector3f leftTop = localCoord.compute(XGL::Orientation::left_top, 18, 18);
	Eigen::Vector3f titlePos = localCoord.compute(XGL::VAlignment::Top, 0, 50);

	auto lengthx = abs(rightBot.x() - leftBot.x());
	auto lengthy = abs(leftTop.y() - leftBot.y());
	Eigen::Vector3f  center = (leftTop + rightBot) * 0.5;


	d->m_title->setPosition(titlePos.x(), titlePos.y());

	{
		d->m_gridItem->setShaderManger(getShaderManger());
		d->m_gridItem->resetTransform();
		d->m_gridItem->translate(center.x(), center.y());
		d->m_gridItem->scale(0.5 * lengthx, 0.5 * lengthy);
	}

	d->m_axisx->getLine()->setLine(XQ::Vec2f(leftBot.x(), leftBot.y()), XQ::Vec2f(rightBot.x(), rightBot.y()));
	d->m_axisy->getLine()->setLine(XQ::Vec2f(leftBot.x(), leftBot.y()), XQ::Vec2f(leftTop.x(), leftTop.y()));


	//图例位置的更新
	{
		auto scenFrameInVirtual = getSceneMatrix();
		LocalCoordCompute localCoord(scenFrameInVirtual * m * selfTransform.matrix());
		auto w = d->m_legend->getWidth();
		auto h = d->m_legend->getHeight();

		auto space = 25;
		//中心点距离左侧的距离是 0.5*w + 间隔
		Eigen::Vector3f center = localCoord.compute(XGL::Orientation::right_top, 0.5 * w + space, 0.5 * h + space);
		auto sx = localCoord.scalex;
		auto sy = localCoord.scaley;
		auto this_sx = w * 0.5 / sx;
		auto this_sy = h * 0.5 / sy;
		d->m_legend->resetTransform();
		d->m_legend->translate(center.x(), center.y());
		d->m_legend->setScale(this_sx, this_sy);
	}
}

void XChartItem2::updateVboCoord()
{
	//顶点数据已经更新
	auto m_coord = m_coordArray;
	if (m_coord && m_coord->GetTimeStamp() > m_UpdateTime) {

		m_vbo_coord->bind();

		if (m_drawType == PrimitveType::line_strip_adjacency) {
			int oldNum = m_coord->getNumOfTuple();
			auto addAdjacency = makeShareDbObject<XFloatArray>();
			addAdjacency->setComponent(3);
			//原始数据只有四个点，添加三个额外的点

			// 1 2 3 4
			// 4 1 2 3 4 1 2
			addAdjacency->setNumOfTuple(oldNum + 3);
			memcpy(addAdjacency->data(1), m_coord->data(0), m_coord->size() * sizeof(float));

			//设置第一个点
			addAdjacency->data(0)[0] = m_coord->data(oldNum - 1)[0];
			addAdjacency->data(0)[1] = m_coord->data(oldNum - 1)[1];
			addAdjacency->data(0)[2] = m_coord->data(oldNum - 1)[2];

			//倒数第二个点
			addAdjacency->data(oldNum + 1)[0] = m_coord->data(0)[0];
			addAdjacency->data(oldNum + 1)[1] = m_coord->data(0)[1];
			addAdjacency->data(oldNum + 1)[2] = m_coord->data(0)[2];

			//最后一个点
			addAdjacency->data(oldNum + 2)[0] = m_coord->data(1)[0];
			addAdjacency->data(oldNum + 2)[1] = m_coord->data(1)[1];
			addAdjacency->data(oldNum + 2)[2] = m_coord->data(1)[2];

			m_vbo_coord->allocate(addAdjacency->data(0), addAdjacency->size());
			m_indexArray->setTuple(0, 1, 2, 3);
			m_indexArray->setTuple(1, 1, 3, 4);
			m_indexArray->Modified();
		}
		else {
			m_vbo_coord->allocate(m_coord->data(0), m_coord->size());
			m_indexArray->setTuple(0, 0, 1, 2);
			m_indexArray->setTuple(1, 0, 2, 3);
			m_indexArray->Modified();
		}

		m_vbo_coord->release();
	}
}

uint32_t XChartItem2::computeNumofVertices()
{
	if(m_drawType == PrimitveType::line_strip_adjacency)
		return m_coordArray->getNumOfTuple()+3;
	else
		return m_coordArray->getNumOfTuple();
}