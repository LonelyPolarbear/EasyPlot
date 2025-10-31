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
	myUtilty::Vec2f m_gridOrigin = myUtilty::Vec2f(0,0);

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

		//此时，XChartItem的大小已经确定，可以根据需要调整原点位置
		//auto chartTransformData = myUtilty::Matrix::transformDecomposition_TRS(chartTransform);
		
		Eigen::Vector3f leftBot = chartTransform * Eigen::Vector3f(-1, -1, 0);
		Eigen::Vector3f rightBot = chartTransform * Eigen::Vector3f(1, -1, 0);
		Eigen::Vector3f leftTop = chartTransform * Eigen::Vector3f(-1, 1, 0);
		Eigen::Vector3f rightTop = chartTransform * Eigen::Vector3f(1, 1, 0);


		leftBot += Eigen::Vector3f(18, 18, 0);
		rightBot += Eigen::Vector3f(-18, 18, 0);

		leftTop += Eigen::Vector3f(18, -18, 0);

		leftBot = chartTransform.inverse() * leftBot;
		rightBot = chartTransform.inverse() * rightBot;
		leftTop = chartTransform.inverse() * leftTop;

		//d->m_gridOrigin = myUtilty::Vec2f(leftBottomPos.x(), leftBottomPos.y());
		

		createGrid();
		if (d->createAxisText()) {
			d->m_axisx->setRange(0,100);
			d->m_axisy->setRange(10,80);
			addChildItem(d->m_axisx);
			addChildItem(d->m_axisy);
			/*for(auto& text : d->m_axisx_value)
				addChildItem(text);
			for (auto& text : d->m_axisy_value)
				addChildItem(text);*/

			//updateAxisLabel();
		}

		d->m_axisx->getLine()->setLine(myUtilty::Vec2f(leftBot.x(), leftBot.y()), myUtilty::Vec2f(rightBot.x(), rightBot.y()));

		d->m_axisy->getLine()->setLine(myUtilty::Vec2f(leftBot.x(), leftBot.y()), myUtilty::Vec2f(leftTop.x(), leftTop.y()));

		d->m_axisx->updateTextPos();
		d->m_axisy->updateTextPos();
		auto gridTranform = /*this->getGridTransform();*/d->m_gridItem->getTransform();

		Eigen::Matrix4f parentTransform1 = m * chartTransform.matrix();

		/*for (auto& text : d->m_axisx_value) {
			text->draw(m);
		}*/
		
		XGraphicsItem::draw(m);

		beginClip(m);

		//d->m_axisx->draw(m);
		//d->m_axisy->draw(m);

		Eigen::Matrix4f parentTransform = m * chartTransform.matrix() * gridTranform.matrix();
		for (auto polyline : d->m_polylines) {
			polyline->draw(parentTransform);
		}

		endClip();

		auto glEnableObj = makeShareDbObject<XOpenGLEnable>();
		glEnableObj->enable(XOpenGLEnable::EnableType::BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableObj->disable(XOpenGLEnable::EnableType::DEPTH_TEST);

		//d->m_gridItem->draw(parentTransform1);
		glEnableObj->restore();
	}
}

void XChartItem2::setBackgroundColor(const myUtilty::Vec4f& color)
{
	m_fillColor = color;
}

void XChartItem2::gridTranslate(float dx, float dy)
{
	createGrid();
}

void XChartItem2::gridSale(float dx, float dy)
{
	createGrid();
	d->m_gridItem->scale(1./dx, 1./dy);

	d->createAxisText();
	updateAxisLabel();
}

void XChartItem2::createGrid()
{
	if (!d->m_gridItem) {
		d->m_gridItem = std::make_shared<XGridItem>();
		d->m_gridItem->setShaderManger(this->getShaderManger());
		//d->m_gridItem->initiallize();
		d->m_gridItem->setIsScreenGrid(false);

		//d->m_gridItem->translate(-0.5, -0.5);
		d->m_gridItem->scale(0.01,0.01);
		d->m_gridItem->setOrigin(d->m_gridOrigin);
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