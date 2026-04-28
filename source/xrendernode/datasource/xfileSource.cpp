#include "xfileSource.h"
#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QFile>
#include <regex>
#include <QSet>


int countRegexMatches(const std::string& str, const std::string& pattern, bool caseSensitive = true) {
	try {
		// 编译正则表达式，根据需求设置是否区分大小写
		//std::regex::flag_type flags = caseSensitive ? std::regex::normal : std::regex::icase;
		std::regex re(pattern);

		// 创建正则迭代器，用于遍历所有匹配
		std::sregex_iterator it(str.begin(), str.end(), re);
		std::sregex_iterator end;  // 迭代器结束标志

		// 迭代器的数量即为匹配次数
		return std::distance(it, end);
	}
	catch (const std::regex_error& e) {
		// 捕获正则表达式语法错误
		std::cerr << "正则表达式错误: " << e.what() << std::endl;
		return -1;
	}
}

std::string readFile2(const std::string& path) {
	std::ifstream ShaderFile;
	ShaderFile.open(path);

	std::stringstream ShaderStream;
	// read file's buffer contents into streams
	ShaderStream << ShaderFile.rdbuf();

	ShaderFile.close();
	return ShaderStream.str();
}

XFileSource::XFileSource()
{
}

XFileSource::~XFileSource()
{
}

void XFileSource::updateVertextCoordArray()
{
	m_VertexCoord->Modified();
}

void XFileSource::updateFaceIndexArray()
{
	m_FaceIndexs->Modified();
}

void XFileSource::updateFaceColorArray()
{
	m_FaceColor->Modified();
}

void XFileSource::updateVertextNormalArray()
{
	m_VertexNormal->Modified();
}

void XFileSource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}

void XFileSource::updateLineIndexArray()
{
}

void XFileSource::updateLineColorArray()
{
}

void XFileSource::updateVertexIndexArray()
{
}

void XFileSource::setFilePath(const std::string& filePath)
{

}


void XFileSource::readFile(const std::string& path)
{
	namespace fs = std::filesystem;
	std::filesystem::path file_path = path;
	if (fs::exists(file_path)) {
		std::cout << "文件存在！\n";

		// 3. 检查是否为常规文件（不是目录）
		if (fs::is_regular_file(file_path)) {
			std::cout << "这是一个文件（非目录）\n";

			// 4. 获取文件信息
			std::cout << "文件名（含扩展名）：" << file_path.filename() << "\n";  // example.txt
			std::cout << "文件名（不含扩展名）：" << file_path.stem() << "\n";     // example
			std::cout << "文件扩展名：" << file_path.extension() << "\n";         // .txt
			std::cout << "绝对路径：" << fs::absolute(file_path) << "\n";         // 完整路径
			std::cout << "文件大小：" << fs::file_size(file_path) << " 字节\n";   // 文件大小
		}
		else {
			std::cout << "这不是一个文件（可能是目录）\n";
		}
	}
	else {
		std::cout << "文件不存在，创建新文件...\n";
	}

	if (file_path.extension() == ".xobj") {
		//读取文件,解析数据
		std::ifstream file(path);

		std::string line;

		std::getline(file, line);

		auto pointNum = std::stoi(line);
		m_VertexCoord->setNumOfTuple(pointNum);

		for (int i = 0; i < pointNum; i++) {
			std::getline(file, line);
			//分割
			auto splitLine = QString::fromStdString(line).split(" ");
			auto x = splitLine[0].toDouble();
			auto y = splitLine[1].toDouble();
			auto z = splitLine[2].toDouble();
			m_VertexCoord->setTuple(i, x, y, z);
		}

		std::getline(file, line);

		auto indexum = std::stoi(line);
		m_FaceIndexs->setNumOfTuple(indexum);

		for (int i = 0; i < indexum; i++) {
			std::getline(file, line);
			auto splitLine = QString::fromStdString(line).split(" ");
			auto v1 = splitLine[0].toInt();
			auto v2 = splitLine[1].toInt();
			auto v3 = splitLine[2].toInt();
			m_FaceIndexs->setTuple(i, v1, v2, v3);
		}

		m_FaceColor->setNumOfTuple(indexum);

		for (int i = 0; i < indexum; i++) {

			m_FaceColor->setTuple(i, 1, 1, 1, 0);
		}
	}
	else if (file_path.extension() == ".inp") {
		auto str = readFile2(path);

		auto num_tri = countRegexMatches(str, "tri");
		auto num_quad = countRegexMatches(str, "quad");		//

		std::ifstream file(path);

		std::string line;

		std::getline(file, line);
		auto splitLine = QString::fromStdString(line).split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
		auto pointNum = splitLine[0].toInt();
		auto indexum = /*splitLine[1].toInt();*/num_quad * 2 + num_tri;

		auto nodeidx2Id = makeShareDbObject< XUIntArray>();
		m_VertexCoord->setNumOfTuple(pointNum);
		nodeidx2Id->setNumOfTuple(pointNum);
		m_FaceIndexs->setNumOfTuple(indexum);

		for (int i = 0; i < pointNum; i++) {
			std::getline(file, line);
			//分割
			auto splitLine = QString::fromStdString(line).split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
			auto nodeId = splitLine[0].toInt();
			nodeidx2Id->setTuple(i,nodeId);
			auto x = splitLine[1].toDouble();
			auto y = splitLine[2].toDouble();
			auto z = splitLine[3].toDouble();
			m_VertexCoord->setTuple(i, x, y, z);
		}

		//std::getline(file, line);

		int idx = 0;
		while (std::getline(file, line)) {
			//std::getline(file, line);
			auto splitLine = QString::fromStdString(line).split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
			if (splitLine[2].toStdString() == "tri") {
				auto v1 = splitLine[3].toInt();
				auto v2 = splitLine[4].toInt();
				auto v3 = splitLine[5].toInt();

				//根据id获取index
				auto begin =nodeidx2Id->data(0);
				
				auto v1_idx = std::distance(begin, std::find(begin, begin + pointNum, v1));
				auto v2_idx = std::distance(begin, std::find(begin, begin + pointNum, v2));
				auto v3_idx = std::distance(begin, std::find(begin, begin + pointNum, v3));

				m_FaceIndexs->setTuple(idx++, v1_idx, v2_idx, v3_idx);
			}
			else {
				auto v1 = splitLine[3].toInt();
				auto v2 = splitLine[4].toInt();
				auto v3 = splitLine[5].toInt();
				auto v4 = splitLine[6].toInt();
				auto begin = nodeidx2Id->data(0);

				auto v1_idx = std::distance(begin, std::find(begin, begin + pointNum, v1));
				auto v2_idx = std::distance(begin, std::find(begin, begin + pointNum, v2));
				auto v3_idx = std::distance(begin, std::find(begin, begin + pointNum, v3));
				auto v4_idx = std::distance(begin, std::find(begin, begin + pointNum, v4));

				m_FaceIndexs->setTuple(idx++, v1_idx, v2_idx, v3_idx);
				m_FaceIndexs->setTuple(idx++, v1_idx, v3_idx, v4_idx);
			}
		}

		m_FaceColor->setNumOfTuple(indexum);


		for (int i = 0; i < indexum; i++) {
			if (i % 3 == 0)
				m_FaceColor->setTuple(i, 1, 0, 0, 1);
			if (i % 3 == 1)
				m_FaceColor->setTuple(i, 0, 1, 0, 1);
			if (i % 3 == 2)
				m_FaceColor->setTuple(i, 0, 0, 1, 1);
		}
	}

	transform();
}

void XFileSource::transform()
{
	auto elemNum =m_FaceIndexs->getNumOfTuple();

	auto pointNum = m_VertexCoord->getNumOfTuple();
	auto pElemIdx = m_FaceIndexs->data(0);

	std::unordered_map<int, std::vector<int>> nodeIdx2ElemIdx;			//顶点的共享单元
	std::vector<Eigen::Vector3f> elemNormal;											//每个单元的法线
	elemNormal.resize(elemNum);

	std::vector<Eigen::Vector3f>	pointNormal;											//计算得到的每个顶点的法线
	pointNormal.resize(pointNum);

	{
		//计算单元法线
		for (int i = 0; i < elemNum; i++) {
			auto v1 = pElemIdx[i * 3 + 0];
			auto v2 = pElemIdx[i * 3 + 1];
			auto v3 = pElemIdx[i * 3 + 2];

			auto e1 = m_VertexCoord->data(v1);
			auto e2 = m_VertexCoord->data(v2);
			auto e3 = m_VertexCoord->data(v3);

			Eigen::Vector3f point1(e1[0], e1[1], e1[2]);
			Eigen::Vector3f point2(e2[0], e2[1], e2[2]);
			Eigen::Vector3f point3(e3[0], e3[1], e3[2]);

			auto normal = (point2 - point1).cross(point3 - point1).normalized();
			elemNormal[i] = normal;
		}
	}

	std::vector<bool> normalDir;
	normalDir.resize(elemNum);

	for(int i=0;i<elemNum;i++){
		normalDir[i] =  elemNormal[i].dot(Eigen::Vector3f(0, 1,0)) > 0;
		if(elemNormal[i].dot(Eigen::Vector3f(0, 1, 0)) < 0){
			elemNormal[i] *=-1;
		}
	}
	
	{
		//获取每个顶点的共享单元
		for (int i = 0; i < pointNum; i++) {
			auto x = m_VertexCoord->data(i);

			//所有共享该节点的单元
			for (int j = 0; j < elemNum; j++) {
				
				if (pElemIdx[j*3+0] == i || pElemIdx[j*3+1] == i || pElemIdx[j*3+2] == i) {
					nodeIdx2ElemIdx[i].push_back(j);
				}
			}
		}
	}

	{
		//计算每个顶点的法线
		//遍历每个节点的连接信息，查找共面单元
		QHash<int, QMap<int, Eigen::Vector3f>> elem_normals;//单元的法线方向 <单元id, <节点id, 法线方向>
		const float one_face_criteria = cos(XQ::Matrix::radian(10));//cos(25) = 0.906，夹角小于25度即认为共面，暂写死
		for (int i = 0; i < pointNum; i++) {
			auto normal = Eigen::Vector3f(0, 0, 0);
			auto elemIdx = nodeIdx2ElemIdx[i];		//该节点的共享单元


			//step1:查找共面单元
			QVector<QVector<int>> one_face_elem_ids;//共面的elem_id	<多组单元id>
			QSet<int> elem_done;//记录已处理单元
			//QSet<int> face_done;//记录已处理的面

			//遍历每个单元
			for (auto j : elemIdx) {
				if (elem_done.contains(j))
					continue;

				elem_done.insert(j);

				QVector<int> one_face_elem_id_j;//与elem_id_id共面的单元
				one_face_elem_id_j.push_back(j);

				//遍历其余共节点单元寻找与单元i共面的单元
				for (auto k : elemIdx)
				{
					if (elem_done.contains(k))continue;

					if (elemNormal[k].dot(elemNormal[j]) > one_face_criteria)
					{
						one_face_elem_id_j.push_back(k);
						elem_done.insert(k);
					}
				}

				one_face_elem_ids.push_back(one_face_elem_id_j);
			}

			//step2:对共面的单元进行法线磨平

			for (auto& one_face_elem_id : one_face_elem_ids)
			{
				Eigen::Vector3f nodenorm = Eigen::Vector3f(0, 0, 0);
				for (auto& elemid : one_face_elem_id)
					nodenorm += elemNormal[elemid];
				nodenorm.normalize();

				float cos = 0;
				for (auto& elemid : one_face_elem_id)
					cos += nodenorm.dot(elemNormal[elemid]);
				cos /= one_face_elem_id.size();

				if(cos !=0)
					nodenorm*=cos;
				else {
					int iii = 0;
				}

				//将数值保存
				for (auto& elemid : one_face_elem_id)
					elem_normals[elemid].insert(i, nodenorm);
			}
		}

		//每一单元都生成新的点E0_P0,E0_P1,E0_P2,E1_P0,E1_P1,E1_P2...
		auto generatePoints = makeShareDbObject< XFloatArray>();		//生成的新的顶点坐标
		generatePoints->setComponent(3);
		generatePoints->setNumOfTuple(elemNum * 3);

		int idx =0;
		for (int i = 0; i < elemNum; i++) {

			//遍历每个单元的节点
			auto node_ids = elem_normals[i].keys();
			//遍历单元，按照顺序生成新的点

			std::vector<unsigned int> ids {m_FaceIndexs->data(i)[0],m_FaceIndexs->data(i)[1] ,m_FaceIndexs->data(i)[2] };


			for (auto& node_id : ids) {
				auto normal = elem_normals[i][node_id];
				//获取该节点的原始位置
				auto x = m_VertexCoord->data(node_id);
				Eigen::Vector3f point(x[0], x[1], x[2]);

				//得到新的节点
				float cos_theta = normal.dot(elemNormal[i]);
				if (cos_theta == 0){
					normal = Eigen::Vector3f(0, 0, 0);
				}

				normal.normalized();
				//Eigen::Vector3f newPoint = point + normal * cos_theta;
				Eigen::Vector3f newPoint = point + normal * 0.2;
				generatePoints->setTuple(idx++, newPoint[0], newPoint[1], newPoint[2]);
			}
		}

		auto generateIndexs = makeShareDbObject< XUIntArray>();		//生成的新的顶点索引
		generateIndexs->setComponent(3);
		generateIndexs->setNumOfTuple(elemNum);

		for (int i = 0; i < elemNum; i++) {
			generateIndexs->setTuple(i, i*3+0+pointNum, i * 3 + 1 + pointNum, i * 3 + 2 + pointNum);
		}

		{
			//扩充顶点
			auto vertexPoints = makeShareDbObject< XFloatArray>();
			vertexPoints->setComponent(3);
			vertexPoints->setNumOfTuple(pointNum + elemNum * 3 );	
			//先拷贝旧的坐标
			memcpy(vertexPoints->data(0), m_VertexCoord->data(0), m_VertexCoord->size() * sizeof(float));
			//拷贝新生成的坐标
			memcpy(vertexPoints->data(pointNum), generatePoints->data(0), generatePoints->size() * sizeof(float));

			//扩充索引
			auto ebo = makeShareDbObject< XUIntArray>();
			ebo->setComponent(3);
			
			ebo->setNumOfTuple(elemNum*2+ elemNum * 6);		//每一个三角形单元，拉伸后，侧面多了6个三角形，所以需要扩充索引
			//先拷贝旧的所用
			memcpy(ebo->data(0), m_FaceIndexs->data(0), m_FaceIndexs->size() * sizeof(unsigned int));
			//拷贝新生成的索引
			memcpy(ebo->data(elemNum), generateIndexs->data(0), generateIndexs->size()*3 * sizeof(unsigned int));

			//扩充侧面的索引
			idx = elemNum * 2;
			for (int i = 0; i < elemNum; i++) {
				auto oldEbo = ebo->data(i);
				auto newEbo = ebo->data(i+elemNum);

				//面1 oldEbo[0]  oldEbo[1]  oldEbo[2]
				//面2 newEbo[0] newEbo[1]  newEbo[2]

				for (int j = 0; j < 3; j++) {
					//j j+1
					//j j+1
					auto fisrt = XQ::Vec3u(oldEbo[j], newEbo[j], oldEbo[(j + 1)%3]);
					auto second = XQ::Vec3u(oldEbo[(j+1)%3], newEbo[j], newEbo[(j+1)%3]);
					ebo->setTuple(idx++, fisrt.x(), fisrt.y(), fisrt.z());
					ebo->setTuple(idx++, second.x(), second.y(), second.z());
				}
			}

			//扩充颜色
			auto faceColor = makeShareDbObject< XFloatArray>();
			faceColor->setComponent(4);
			faceColor->setNumOfTuple(elemNum * 2+elemNum*6);

			//先拷贝旧的颜色
			memcpy(faceColor->data(0), m_FaceColor->data(0), m_FaceColor->size() * sizeof(float));
			//拷贝新生成的颜色
			for (int i = 0; i < elemNum; i++) {
				faceColor->setTuple(i + elemNum, 1, 1, 0, 1.0f);
			}

			for (int i = 0; i < 6*elemNum; i++) {
				faceColor->setTuple(i + 2*elemNum, 0, 1, 1, 1.0f);
			}

			m_VertexCoord = vertexPoints;
			m_FaceIndexs = ebo;
			m_FaceColor = faceColor;
		}
	}
}