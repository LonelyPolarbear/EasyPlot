#include "xshape.h"
#include <lib04_opengl/XOpenGLBuffer.h>
#include <lib04_opengl/XOpenGLVertexArrayObject.h>
#include <lib00_utilty/myUtilty.h>
#include <lib01_shader/xshader.h>
#include <glew/glew.h>

#include "xsphereSource.h"

#include <QFile> 
#include <QVector>

struct PlotData {
	double phi;
	double theta;
	double Gain;
};

class XSphereSource::Internal {
public:
	double m_radius{ 1 };
	std::pair<double, double> EWRange = { 0,360 };       // 东西 对应θ
	std::pair<double, double> SNRange = { 0,180 };          //南北,对应 phi
	unsigned EWNum = 360;           //经度分成多少份
	unsigned SNNum = 180;           //纬度分成多少份

	Direaction direction = Direaction::horizontal;

	double getEWStepLen() {
		return (EWRange.second - EWRange.first) / (double)EWNum;
	}

	double getSNStepLen() {
		return (SNRange.second - SNRange.first) / (double)SNNum;
	}

	std::vector<myUtilty::Vec3f> generatePoints() {
		auto LongitudeStepLen = getEWStepLen();
		auto LatitudeStepLen = getSNStepLen();

		auto pointNum = getSurfaceNumberPoints();
		std::vector<myUtilty::Vec3f> points;
		points.resize(pointNum);

		//先遍历维度
		if (direction == Direaction::horizontal) {
			for (int i = 0; i <= SNNum; i++)
			{
				auto zAngle = SNRange.first + i * LatitudeStepLen;

				for (int j = 0; j <= EWNum; j++) { //0-360
					auto idx = j * (SNNum + 1) + i;
					auto xAngle = EWRange.first + j * LongitudeStepLen;
					auto r =/* abs( csvData[idx].Gain)*/m_radius;
					double z = r * cos(zAngle * myUtilty::PI / 180.0);
					double x = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle * myUtilty::PI / 180.0);
					double y = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle * myUtilty::PI / 180.0);
					points[i * (EWNum + 1) + j] = myUtilty::Vec3f(x, y, z);
				}
			}
		}
		else if (direction == Direaction::vertical) {
			for (int i = 0; i <= EWNum; i++)
			{
				auto xAngle = EWRange.first + i * LongitudeStepLen;
				for (int j = 0; j <= SNNum; j++) { //0-180
					//auto idx =i * (EWNum + 1) + i;
					auto zAngle = SNRange.first + j * LatitudeStepLen;
					auto r = m_radius;
					double z = r * cos(zAngle * myUtilty::PI / 180.0);
					double x = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle * myUtilty::PI / 180.0);
					double y = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle * myUtilty::PI / 180.0);
					points[i * (SNNum + 1) + j] = myUtilty::Vec3f(x, y, z);
				}
			}
		}
		return points;
	}

	std::vector<myUtilty::Vec3u> generateIndexs() {
		std::vector<myUtilty::Vec3u> indices;
		indices.resize(getSurfaceNumberOfTriangle());
		int index = 0;
		if (direction == Direaction::horizontal)
		{
			//南北
			for (int i = 1; i < (SNNum + 1); i++)
			{
				auto firstRowSrart = (i - 1) * (EWNum + 1);
				auto secondRowStart = i * (EWNum + 1);
				for (int j = 0; j < EWNum; j++) {
					myUtilty::Vec3u fisrttriangle = myUtilty::Vec3u(firstRowSrart + j, secondRowStart + j, secondRowStart + j + 1);
					myUtilty::Vec3u secondtriangle = myUtilty::Vec3u(firstRowSrart + j, secondRowStart + j + 1, (firstRowSrart + 1) + j);
					indices[index++] = fisrttriangle;
					indices[index++] = secondtriangle;
				}
			}
		}
		else if (direction == Direaction::vertical)
		{
			//东西
			for (int i = 1; i < (EWNum + 1); i++)
			{
				auto firstRowSrart = (i - 1) * (SNNum + 1);
				auto secondRowStart = i * (SNNum + 1);
				for (int j = 0; j < SNNum; j++) {
					myUtilty::Vec3u fisrttriangle = myUtilty::Vec3u(firstRowSrart + j, secondRowStart + j, secondRowStart + j + 1);
					myUtilty::Vec3u secondtriangle = myUtilty::Vec3u(firstRowSrart + j, secondRowStart + j + 1, (firstRowSrart + 1) + j);
					indices[index++] = fisrttriangle;
					indices[index++] = secondtriangle;
				}
			}
		}

		return indices;
	}

	//补东西缺口
	std::vector<myUtilty::Vec3u> generateEWRestIndexs(unsigned int offset) {
		std::vector<myUtilty::Vec3u> indices;

		auto SNStepLen = getSNStepLen();
		auto num = std::ceil((SNRange.second - SNRange.first) / SNStepLen);
		//每一条弧一共num+2个点 原点 点0 点1 ...
		//对于第一条弧,一共可以绘制num个三角形
		indices.resize(getEWNumberOfTriangle());
		if (indices.size() == 0)
			return indices;
		int idx = 0;
		for (int i = 1; i <= num; i++) {
			myUtilty::Vec3u firsttriangle = myUtilty::Vec3u(offset, offset + i, offset + i + 1);
			indices[idx++] = firsttriangle;
		}

		//对于第二条弧,一共可以绘制num个三角形
		auto secondOffset = offset + num + 2;
		for (int i = 1; i <= num; i++) {
			myUtilty::Vec3u firsttriangle = myUtilty::Vec3u(secondOffset, secondOffset + i, secondOffset + i + 1);
			indices[idx++] = firsttriangle;
		}

		//(0 1 3), (0, 2, 4), (0, 1, 5), (0, 2, 6)
		auto curIdx = secondOffset + num + 1;
		curIdx++;
		indices[idx++] = myUtilty::Vec3u(curIdx + 0, curIdx + 1, curIdx + 3);
		indices[idx++] = myUtilty::Vec3u(curIdx + 0, curIdx + 2, curIdx + 4);
		indices[idx++] = myUtilty::Vec3u(curIdx + 0, curIdx + 1, curIdx + 5);
		indices[idx++] = myUtilty::Vec3u(curIdx + 0, curIdx + 2, curIdx + 6);
		return indices;
	}

	//EW结束后剩下的是SN的部分
	std::vector<myUtilty::Vec3f> getEWRestPoints() {
		std::vector<myUtilty::Vec3f> points;

		//获取起点和终点的圆弧
		auto SNStepLen = getSNStepLen();
		auto num = std::ceil((SNRange.second - SNRange.first) / SNStepLen);
		auto newStepLen = (SNRange.second - SNRange.first) / num;

		//int vecNum =(1 + num+1)*2+7;

		int vecNum = getNumofEWRestPoint();
		std::vector<myUtilty::Vec3f> points1;
		std::vector<myUtilty::Vec3f> points2;

		points1.push_back(myUtilty::Vec3f(0, 0, 0));		//1
		points2.push_back(myUtilty::Vec3f(0, 0, 0));
		myUtilty::Vec3f SNStartPointTop;
		myUtilty::Vec3f SNStartPointBottom;

		myUtilty::Vec3f SNEndPointTop;
		myUtilty::Vec3f SNEndPointBottom;

		myUtilty::Vec3f centerTop;
		myUtilty::Vec3f centerBottom;

		for (int i = 0; i < num; i++) {									//num
			auto zAngle = SNRange.first + i * newStepLen;
			auto xAngle1 = EWRange.first;
			auto xAngle2 = EWRange.second;
			auto r =/* abs( csvData[idx].Gain)*/m_radius;
			double z1 = r * cos(zAngle * myUtilty::PI / 180.0);
			double x1 = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle1 * myUtilty::PI / 180.0);
			double y1 = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle1 * myUtilty::PI / 180.0);

			double x2 = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle2 * myUtilty::PI / 180.0);
			double y2 = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle2 * myUtilty::PI / 180.0);

			points1.push_back(myUtilty::Vec3f(x1, y1, z1));
			points2.push_back(myUtilty::Vec3f(x2, y2, z1));

			if (i == 0)
			{
				SNStartPointTop = myUtilty::Vec3f(x1, y1, z1);
				SNEndPointTop = myUtilty::Vec3f(x2, y2, z1);
				centerTop = myUtilty::Vec3f(0, 0, z1);
			}
		}
		//还差最后一个点
		{
			auto zAngle = SNRange.second;
			auto xAngle1 = EWRange.first;
			auto xAngle2 = EWRange.second;
			auto r =/* abs( csvData[idx].Gain)*/m_radius;
			double z1 = r * cos(zAngle * myUtilty::PI / 180.0);
			double x1 = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle1 * myUtilty::PI / 180.0);
			double y1 = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle1 * myUtilty::PI / 180.0);
			double x2 = r * sin(zAngle * myUtilty::PI / 180.0) * cos(xAngle2 * myUtilty::PI / 180.0);
			double y2 = r * sin(zAngle * myUtilty::PI / 180.0) * sin(xAngle2 * myUtilty::PI / 180.0);
			points1.push_back(myUtilty::Vec3f(x1, y1, z1));											//1
			points2.push_back(myUtilty::Vec3f(x2, y2, z1));
			{
				SNStartPointBottom = myUtilty::Vec3f(x1, y1, z1);
				SNEndPointBottom = myUtilty::Vec3f(x2, y2, z1);
				centerBottom = myUtilty::Vec3f(0, 0, z1);
			}
		}

		//合并两个数组

		//6个点 四个三角形 (0 1 3),(0,2,4),(0,1,5),(0,2,6)  
		myUtilty::Vec3f center = myUtilty::Vec3f(0, 0, 0);
		//点的判断
		if (centerTop.z() * centerBottom.z() < 0) {
			center = myUtilty::Vec3f(0, 0, 0);
		}
		else if (centerTop.z() > 0) {
			//说明都在上面
			center = centerBottom;
		}
		else {
			//说明都在下面
			center = centerTop;
		}

		points1[0] = center;
		points2[0] = center;

		points.insert(points.end(), points1.begin(), points1.end());
		points.insert(points.end(), points2.begin(), points2.end());

		//7
		points.push_back(center);
		points.push_back(centerTop);
		points.push_back(centerBottom);
		points.push_back(SNStartPointTop);
		points.push_back(SNStartPointBottom);
		points.push_back(SNEndPointTop);
		points.push_back(SNEndPointBottom);


		return points;
		//从 SNRange.first SNRange.second,间隔
	}

	//补上下的缺口
	std::vector<myUtilty::Vec3f> getSNRestPoints() {
		std::vector<myUtilty::Vec3f> points;
		//容器总大小
		int vecNum = getNumofSNRestPoint();
		double z1 = m_radius * cos(SNRange.first * myUtilty::PI / 180.0);
		double z2 = m_radius * cos(SNRange.second * myUtilty::PI / 180.0);
		double r_top = abs(m_radius * sin(SNRange.first * myUtilty::PI / 180.0));
		double r_bottom = abs(m_radius * sin(SNRange.second * myUtilty::PI / 180.0));

		myUtilty::Vec3f centerTop = myUtilty::Vec3f(0, 0, z1);
		myUtilty::Vec3f centerBottom = myUtilty::Vec3f(0, 0, z2);

		//EWRange.first 到 EWRange.second 之间 绘制的点
		std::vector<myUtilty::Vec3f> pointsTop;
		std::vector<myUtilty::Vec3f> pointsBottom;
		pointsTop.push_back(centerTop);																//1
		pointsBottom.push_back(centerBottom);

		for (int i = 0; i < EWNum; i++) {																//EWNum
			auto xAngle = EWRange.first + i * getEWStepLen();
			double x = r_top * cos(xAngle * myUtilty::PI / 180.0);
			double y = r_top * sin(xAngle * myUtilty::PI / 180.0);

			double x2 = r_bottom * cos(xAngle * myUtilty::PI / 180.0);
			double y2 = r_bottom * sin(xAngle * myUtilty::PI / 180.0);

			pointsTop.push_back(myUtilty::Vec3f(x, y, z1));
			pointsBottom.push_back(myUtilty::Vec3f(x2, y2, z2));
		}

		{
			//最后一个点单独处理，保证精度																	//1
			auto xAngle = EWRange.second;
			double x = r_top * cos(xAngle * myUtilty::PI / 180.0);
			double y = r_top * sin(xAngle * myUtilty::PI / 180.0);

			double x2 = r_bottom * cos(xAngle * myUtilty::PI / 180.0);
			double y2 = r_bottom * sin(xAngle * myUtilty::PI / 180.0);
			pointsTop.push_back(myUtilty::Vec3f(x, y, z1));
			pointsBottom.push_back(myUtilty::Vec3f(x2, y2, z2));
		}

		points.insert(points.end(), pointsTop.begin(), pointsTop.end());
		points.insert(points.end(), pointsBottom.begin(), pointsBottom.end());

		return points;
	}

	std::vector<myUtilty::Vec3u> generateSNRestIndexs(unsigned int offset) {
		std::vector<myUtilty::Vec3u> indices;
		int vecNum = getSNNumberOfTriangle();
		for (int i = 0; i < EWNum; i++) {
			myUtilty::Vec3u firsttriangle = myUtilty::Vec3u(offset, offset + i + 1, offset + i + 2);
			indices.push_back(firsttriangle);
		}

		auto secondOffset = offset + EWNum + 2;

		for (int i = 0; i < EWNum; i++) {
			myUtilty::Vec3u firsttriangle = myUtilty::Vec3u(secondOffset, secondOffset + i + 1, secondOffset + i + 2);
			indices.push_back(firsttriangle);
		}

		return indices;
	}

	unsigned int gteNumberOfTriangle() {
		return getEWNumberOfTriangle() + getSNNumberOfTriangle() + getSurfaceNumberOfTriangle();
	}

	unsigned int getSurfaceNumberOfTriangle() {
		return EWNum * SNNum * 2;
	}

	unsigned int getSurfaceNumberPoints() {
		return (SNNum + 1) * (EWNum + 1);
	}

	unsigned int getNumofSNRestPoint() {
		return (1 + EWNum + 1) * 2;
	}

	unsigned int getNumofEWRestPoint() {
		auto SNStepLen = getSNStepLen();
		auto num = std::ceil((SNRange.second - SNRange.first) / SNStepLen);
		auto newStepLen = (SNRange.second - SNRange.first) / num;

		int vecNum = (1 + num + 1) * 2 + 7;
		return vecNum;
	}

	unsigned int getEWNumberOfTriangle() {
		//return (SNNum+2)*2;
		auto SNStepLen = getSNStepLen();
		if (SNStepLen == (double)0)
			return 0;
		auto num = std::ceil((SNRange.second - SNRange.first) / SNStepLen);
		return 2 * num + 4;
	}

	unsigned int getSNNumberOfTriangle() {
		return EWNum * 2;
	}

	void setSNRange(double start, double end) {
		if (start < 0)
			start = 0;
		if (end > 180)
			end = 180;

		SNRange.first = start;
		SNRange.second = end;
	}

	void setEWRange(double start, double end) {
		if (start < 0)
			start = 0;
		if (end > 360)
			end = 360;
		EWRange.first = start;
		EWRange.second = end;
	}

	void setSNNum(int num) {
		if (num < 2)
			num = 2;
		SNNum = num;
	}

	void setEWNum(int num) {
		if (num < 2)
			num = 2;
		EWNum = num;
	}

	bool isValid() const {
		if (EWRange.first >= EWRange.second || SNRange.first >= SNRange.second) {
			return false;
		}
		return true;
	}


	//---------------------------------------------------------------------------------------
	void readCSV() {
		//读取CSV文件

		QFile file(QString::fromStdString(myUtilty::ShareVar::instance().currentExeDir + "/test.csv"));
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return;
		}

		QString content = file.readAll();

		file.close();

		QStringList parts = content.split(QRegExp("\\n"));

		QVector<PlotData> tmpdata;


		//读取到的是全部的数值

		for (int i = 1; (i + 1) < parts.size(); i++) {
			PlotData pd;
			auto fff = parts.at(i);
			auto str = fff.split(",");
			pd.phi = str.at(0).toDouble();
			pd.theta = str.at(1).toDouble();
			pd.Gain = str.at(2).toDouble();
			tmpdata.push_back(pd);
			gainRange.first = std::min(gainRange.first, pd.Gain);
			gainRange.second = std::max(gainRange.second, pd.Gain);
		}

		//从data中提取phi范围和theta范围 

		{
			for (auto tmp : tmpdata) {
				if (
					EWRange.first <= tmp.theta && tmp.theta <= EWRange.second &&
					SNRange.first <= tmp.phi && tmp.phi <= SNRange.second
					) {
					csvData.push_back(tmp);
				}
				else {
					int i = 0;
				}
			}
		}

		range = gainRange.second - gainRange.first;
	}
	///如果每个点的半径不同
	QVector<PlotData> csvData;
	std::pair<double, double> gainRange = { std::numeric_limits<double>::max(),std::numeric_limits<double>::lowest() };
	double range;
};


XSphereSource::XSphereSource() : d(new Internal())
{
	//d->readCSV(); 
}

XSphereSource::~XSphereSource()
{
}

void XSphereSource::setRadius(double r)
{
	d->m_radius = r;
}

void XSphereSource::setDirection(Direaction dire)
{
	d->direction = dire;
}

unsigned int XSphereSource::getDirectionTriangleNum() const
{
	if (d->direction == Direaction::horizontal) {
		return 2 * d->EWNum;
	}
	else if (d->direction == Direaction::vertical) {
		return 2 * d->SNNum;
	}
}

void XSphereSource::setSNRange(double start, double end)
{
	d->setSNRange(start, end);
}

void XSphereSource::setEWRange(double start, double end)
{
	d->setEWRange(start, end);
}

void XSphereSource::setSNNum(int num)
{
	d->setSNNum(num);
}

void XSphereSource::setEWNum(int num)
{
	d->setEWNum(num);
}

unsigned int XSphereSource::gteNumberOfTriangle() const
{
	return d->gteNumberOfTriangle();
}

unsigned int XSphereSource::getSurfaceNumberOfTriangle() const
{
	return d->getSurfaceNumberOfTriangle();
}

unsigned int XSphereSource::getEWNumberOfTriangle() const
{
	return d->getEWNumberOfTriangle();
}

unsigned int XSphereSource::getSNNumberOfTriangle() const
{
	return d->getSNNumberOfTriangle();
}

unsigned int XSphereSource::getSurfaceNumberPoints() const
{
	return d->getSurfaceNumberPoints();
}

unsigned int XSphereSource::getNumofSNRestPoint() const
{
	return d->getNumofSNRestPoint();
}

unsigned int XSphereSource::getNumofEWRestPoint() const
{
	return d->getNumofEWRestPoint();
}

void XSphereSource::updateVertextCoordArray()
{
	std::vector<myUtilty::Vec3f> points = d->generatePoints();

	auto pointNum = points.size();

	auto restPoints = d->getEWRestPoints();		//东西方向剩余的部分

	auto restPointsSN = d->getSNRestPoints();

	points.insert(points.end(), restPoints.begin(), restPoints.end());

	points.insert(points.end(), restPointsSN.begin(), restPointsSN.end());


	m_coord->setNumOfTuple(points.size());

	float* pVertexData = m_coord->data(0);

	memcpy(pVertexData, points.data(), points.size() * 3 * sizeof(float));

	m_coord->Modified();
}

void XSphereSource::updateIndexArray()
{
	//获取ebo数据
	std::vector<myUtilty::Vec3u> indices = d->generateIndexs();

	auto restIndices = d->generateEWRestIndexs(getSurfaceNumberPoints());

	auto restIndicesSN = d->generateSNRestIndexs(getSurfaceNumberPoints() + getNumofEWRestPoint());

	indices.insert(indices.end(), restIndices.begin(), restIndices.end());

	indices.insert(indices.end(), restIndicesSN.begin(), restIndicesSN.end());

	m_indexs->setNumOfTuple(indices.size());

	unsigned int* pIndexData = m_indexs->data(0);

	memcpy(pIndexData, indices.data(), indices.size() * 3 * sizeof(unsigned int));

	m_indexs->Modified();
}

void XSphereSource::updateFaceColorArray()
{
	//SSBO数据
	auto totalNum = d->gteNumberOfTriangle(); //一共这么多三角形
	m_FaceColor->setNumOfTuple(totalNum);

	auto surfaceNum = getSurfaceNumberOfTriangle();
	auto ewTriangleNum = getEWNumberOfTriangle();
	auto snTriangleNum = getSNNumberOfTriangle();
	float* pFaceColor = m_FaceColor->data(0);

	for (int i = 0; i < surfaceNum; i++) {
		pFaceColor[i * 4 + 0] = 1.f;
		pFaceColor[i * 4 + 1] = 1.f;
		pFaceColor[i * 4 + 2] = 0.f;
		pFaceColor[i * 4 + 3] = 1.0f;
	}

	for (int i = 0; i < ewTriangleNum; i++) {
		pFaceColor[(surfaceNum + i) * 4 + 0] = 0.0f;
		pFaceColor[(surfaceNum + i) * 4 + 1] = 1.0f;
		pFaceColor[(surfaceNum + i) * 4 + 2] = 0.0f;
		pFaceColor[(surfaceNum + i) * 4 + 3] = 1.0f;
	}

	for (int i = 0; i < snTriangleNum; i++) {
		pFaceColor[(surfaceNum + ewTriangleNum) * 4 + i * 4 + 0] = 0.0f;
		pFaceColor[(surfaceNum + ewTriangleNum) * 4 + i * 4 + 1] = 0.0f;
		pFaceColor[(surfaceNum + ewTriangleNum) * 4 + i * 4 + 2] = 1.0f;
		pFaceColor[(surfaceNum + ewTriangleNum) * 4 + i * 4 + 3] = 1.0f;
	}

	m_FaceColor->Modified();
}

void XSphereSource::updateNormalArray()
{

}

void XSphereSource::updateVertexColorArray()
{
}
