#include "XStorage.h"
#include <highfive/H5File.hpp>
#include <highfive/H5Easy.hpp>
#include <iostream>
#include <vector>
namespace XQ {
	XStorage::XStorage()
	{

	}

	void XStorage::test()
	{
		// 创建一个新文件
		HighFive::File file("test.h5", HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);

		file.createGroup("/root/hgh2");

		return;
		//// 写入一个 vector
		std::vector<int> data = { 1, 2, 3, 4, 5 };
		file.createDataSet("/my_dataset", data);

		// 读取并验证
		auto dataset = file.getDataSet("/my_dataset");
		std::vector<int> read_data;
		dataset.read(read_data);

		std::cout << "Read back: ";
		for (int x : read_data) std::cout << x << " ";
		std::cout << std::endl;

		//// 使用 H5Easy 简化操作（可选）
		//H5Easy::dump(file, "/easy_data", 42);
		//int val = H5Easy::load<int>(file, "/easy_data");
		//std::cout << "H5Easy value: " << val << std::endl;
	}
}
