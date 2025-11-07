#include "XChartItem2.h"
#include "XGridItem.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include "lib05_shape/XPolyline.h"
#include <Eigen/Eigen>

#include <lib04_opengl/XOpenGLEnable.h>
#include "lib05_shape/XTextItem.h"
#include "lib05_shape/XAxisItem.h"
#include "lib05_shape/XLineItem.h"
#include <Eigen/Eigen>


class XChartItem2::Internal {
public:
	std::vector<std::shared_ptr<XGraphicsItem>> m_polylines;	//线段集合
	std::shared_ptr< XGridItem> m_gridItem;	//网格
	sptr<XAxisItem> m_axisx;	//X轴
	sptr<XAxisItem> m_axisy;	//Y轴

	int m_xlabelNum =11;					//X轴标签数量
	int m_ylabelNum =4;						//Y轴标签数量
	bool m_isShowGrid = true;			//是否显示网格

	myUtilty::Vec2f mXRange = myUtilty::Vec2f(-1, 1);
	myUtilty::Vec2f mYRange = myUtilty::Vec2f(-1, 1);


	bool createAxisText() {
		if (m_axisx == nullptr || m_axisy == nullptr) {
			m_axisy = makeShareDbObject<XAxisItem>();
			m_axisy->setLayout(XGL::Layout::vertical);
			m_axisy->updateTextPos();

			m_axisx = makeShareDbObject<XAxisItem>();
			m_axisx->setLayout(XGL::Layout::horizontal);
			m_axisx->updateTextPos();

			m_axisx->getLine()->setSingleColor(myUtilty::Vec4f(1, 0, 0, 1));
			m_axisy->getLine()->setSingleColor(myUtilty::Vec4f(0, 1, 0, 1));

			m_axisx->getLine()->setFixedLine(true);
			m_axisy->getLine()->setFixedLine(true);

			m_axisx->getLine()->setLineWidth(3);
			m_axisy->getLine()->setLineWidth(3);
			return true;
		}
		return false;
	}
};

XChartItem2::XChartItem2():XGraphicsItem(),d(new Internal())
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

	this->setIsFilled(false);

	this->setBackgroundColor(myUtilty::Vec4f(	0.f, 0.f, 0.f, 1.0f));

	m_isShowGrid = true;
	m_clipEnable = true;
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
	}
}

void XChartItem2::clearPolylines()
{
	m_childItems.clear();
}

void XChartItem2::draw(const Eigen::Matrix4f& m)
{
	initiallize();
	{
		//对于自身的绘制，先更新数据
		auto chartTransform = this->getTransform();

		Eigen::Vector3f leftBot = chartTransform * Eigen::Vector3f(-1, -1, 0);
		Eigen::Vector3f rightBot = chartTransform * Eigen::Vector3f(1, -1, 0);
		Eigen::Vector3f leftTop = chartTransform * Eigen::Vector3f(-1, 1, 0);
		Eigen::Vector3f rightTop = chartTransform * Eigen::Vector3f(1, 1, 0);

		Eigen::Vector3f dir_x =(rightBot - leftBot).normalized();
		Eigen::Vector3f dir_y = (leftTop - leftBot).normalized();

		leftBot += 18*dir_x;
		rightBot -= 18 * dir_x;
		leftTop += 18 * dir_x;

		leftBot = chartTransform.inverse() * leftBot;
		rightBot = chartTransform.inverse() * rightBot;
		leftTop = chartTransform.inverse() * leftTop;

		auto lengthx = abs(rightBot.x() - leftBot.x());
		auto lengthy = abs(leftTop.y() - leftBot.y());
		Eigen::Vector3f  center = (leftTop+rightBot)*0.5;

	
		createGrid();
		if (d->createAxisText()) {
			d->m_axisx->setRange(-40,100);
			d->m_axisy->setRange(-20,80);

			d->m_axisx->setRange(d->mXRange.x, d->mXRange.y);
			d->m_axisy->setRange(d->mYRange.x, d->mYRange.y);

			addChildItem(d->m_axisx);
			addChildItem(d->m_axisy);
			updateGridFrame();
		}

		{
			d->m_gridItem->resetTransform();
			d->m_gridItem->translate(center.x(), center.y());
			d->m_gridItem->scale(0.5 * lengthx, 0.5 * lengthy);
		}

		d->m_axisx->getLine()->setLine(myUtilty::Vec2f(leftBot.x(), leftBot.y()), myUtilty::Vec2f(rightBot.x(), rightBot.y()));

		d->m_axisy->getLine()->setLine(myUtilty::Vec2f(leftBot.x(), leftBot.y()), myUtilty::Vec2f(leftTop.x(), leftTop.y()));

		d->m_axisx->updateTextPos();

		d->m_axisy->updateTextPos();

		XGraphicsItem::draw(m);

		auto clipTransform = chartTransform * d->m_gridItem->getTransform() * chartTransform.inverse();
		beginClip(m*clipTransform.matrix());

		Eigen::Matrix4f lineParentTransform = m * chartTransform.matrix() *d->m_gridItem->getTransform().matrix()* d->m_gridItem->getGridTransform().matrix();
		for (auto polyline : d->m_polylines) {
			polyline->draw(lineParentTransform);
		}

		endClip();

		auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);

		Eigen::Matrix4f parentTransform = m * chartTransform.matrix();
		d->m_gridItem->draw(parentTransform);
		glEnableObj->restore();
	}
}

void XChartItem2::fitView()
{
	for(auto line : d->m_polylines){
		auto box = line->getBoundBox();
		d->mXRange.x = std::min(d->mXRange.x,box.xmin);
		d->mXRange.y = std::max(d->mXRange.y, box.xmax);

		d->mYRange.x = std::min(d->mYRange.x, box.ymin);
		d->mYRange.y = std::max(d->mYRange.y, box.ymax);
	}
}

void XChartItem2::setBackgroundColor(const myUtilty::Vec4f& color)
{
	m_fillColor = color;
}

void XChartItem2::chartTranslate(const myUtilty::Vec2f& lastPos_, const myUtilty::Vec2f& curPos_)
{
	auto parentTran = getParentAccumulateTransform();
	
	//表达的是网格坐标中的点到场景坐标系的变换
	auto ssss = d->m_gridItem->getGridTransform();
	auto chartTransform =getTransform()*d->m_gridItem->getTransform()*d->m_gridItem->getGridTransform();

	Eigen::Affine3f scene2Chart = chartTransform.inverse();
	Eigen::Vector3f curPos = scene2Chart * Eigen::Vector3f(curPos_.x, curPos_.y, 0);
	Eigen::Vector3f lasPos = scene2Chart * Eigen::Vector3f(lastPos_.x, lastPos_.y, 0);

	auto dx = curPos.x() - lasPos.x();
	auto dy = curPos.y() - lasPos.y();
	createGrid();
	d->createAxisText();
	auto range =d->m_axisx->getRange();
	range.x -= dx;
	range.y -=dx;
	d->m_axisx->setRange(range.x, range.y);

	{
		auto range = d->m_axisy->getRange();
		range.x -= dy;
		range.y -= dy;
		d->m_axisy->setRange(range.x, range.y);
	}
	
	updateGridFrame();
}

void XChartItem2::chartSale(float dx, float dy)
{
	createGrid();
	{
		dx = dx > 1 ? 0.1 : -0.1;
		dy = dy > 1 ? 0.1 : -0.1;

		auto range = d->m_axisx->getRange();
		auto len = range.y - range.x;
		range.x -= dx * len;
		range.y += dy * len;
		d->m_axisx->setRange(range.x, range.y);
	}
	{
		auto range = d->m_axisy->getRange();
		auto len = range.y - range.x;
		range.x -= dy * len;
		range.y += dx * len;
		d->m_axisy->setRange(range.x, range.y);
	}
	
	d->createAxisText();
	updateGridFrame();
}

myUtilty::Vec2d XChartItem2::getOrigin() const
{
	return myUtilty::Vec2d(d->m_axisx->getRange().x, d->m_axisy->getRange().x);
}

void XChartItem2::updateGridFrame()
{
	//当轴的范围变化时，需要更新网格坐标系
	createGrid();
	auto sx = d->m_axisx->getRange().y - d->m_axisx->getRange().x;
	auto sy = d->m_axisy->getRange().y - d->m_axisy->getRange().x;
	d->m_gridItem->gridReset();
	d->m_gridItem->gridSetSale(sx, sy);
	auto origin = myUtilty::Vec2d(d->m_axisx->getRange().x, d->m_axisy->getRange().x);
	d->m_gridItem->setOrigin(origin);

	updateAxisLabel();
}

void XChartItem2::createGrid()
{
	if (!d->m_gridItem) {
		d->m_gridItem = std::make_shared<XGridItem>();
		d->m_gridItem->setShaderManger(this->getShaderManger());
		d->m_gridItem->setIsScreenGrid(false);
	}
}

void XChartItem2::updateAxisLabel()
{
#if 0
	//获取网格的变换矩阵
	auto gridTransform = d->m_gridItem->getTransform();
	auto data = myUtilty::Matrix::transformDecomposition_TRS(gridTransform);

	for (int i = 0; i < d->m_axisx_value.size(); i++) {
		auto xaxis = d->m_axisx_value[i];

		auto length = 1. / data.sx;
		auto step = length / (d->m_xlabelNum - 1);
		double value = -0.5 * length + i * step;

		xaxis->setText(myUtilty::to_wstring_with_precision(value, 1));
	}

	for (int i = 0; i < d->m_axisy_value.size(); i++) {
		auto xaxis = d->m_axisy_value[i];
		auto length = 1. / data.sy;
		auto step = length / (d->m_ylabelNum - 1);
		double value = -0.5 * length + i * step;
		//double v = (i - 2) * 0.5 * 1. / data.sy;
		xaxis->setText(myUtilty::to_wstring_with_precision(value, 1));
	}
	#endif
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