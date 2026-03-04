#include <iostream>
#include <dataBase/XVector.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataArray.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <numeric>

void testXVector() {
	XQ::print("一维数组");

	auto tupleNum = 12;
	auto array1d = makeShareDbObject<XIntArray>();
	array1d->setComponent(1);
	array1d->setNumOfTuple(tupleNum);

	auto pdata = array1d->data(0);
	std::iota(pdata, pdata + tupleNum, 0);

	array1d->dump();

	XQ::print("一维数组转二维，组分相同");
	auto array2d = makeShareDbObject<XIntArray2D>();
	array2d->setComponent(1);
	array2d->setDimensions(4, 3);
	array2d->memCopy(array1d);
	array2d->dump();

	XQ::print("一维数组转二维，组分不同");
	{
		auto array2d = makeShareDbObject<XIntArray2D>();
		array2d->setComponent(4);
		array2d->setDimensions(1, 4);
		array2d->memCopy(array1d);
		array2d->dump();
	}

	XQ::print("一维数组转二维，组分不同");
	{
		auto array2d = makeShareDbObject<XDataArray2D<XQ::Vec2i>>();
		array2d->setComponent(1);
		array2d->setDimensions(2, 4);
		array2d->memCopy(array1d);
		array2d->dump();
	}
}

void testXDataObject01() {
	auto root = makeShareDbObject<XDataObject>(); root->setName("root");
	auto child1 = makeShareDbObject<XDataObject>();child1->setName("child1");
	auto child2 = makeShareDbObject<XDataObject>(); child2->setName("child2");

	root->addData(child1);
	root->addData(child2);

	auto child1_1 = makeShareDbObject<XDataObject>(); child1_1->setName("child1_1");
	auto child1_2 = makeShareDbObject<XDataObject>(); child1_2->setName("child1_2");

	XQ::print("-------------------------");

	child1->addData(child1_1);
	child1->addData(child1_2);

	child1->beginBatch();
	child1->AttrVisible->setValue(false);
	child1_1->AttrVisible->setValue(false);

	child1->endBatch();

	child1->removeData(child1_1);

}

void testXDataObject02(){
	auto attr_bool = makeShareDbObject<XAttr_Bool>();
	auto attr_point = makeShareDbObject<XAttr_Point4f>();
	XQ::print(attr_bool->getClassName());
	XQ::print(attr_point->getClassName());
}

int main() {
	//testXDataObject01();
	testXDataObject02();
	return 1;
}