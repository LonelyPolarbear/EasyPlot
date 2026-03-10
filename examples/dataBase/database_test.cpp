#include <iostream>
#include <dataBase/XVector.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <dataBase/XDataArray.h>
#include <dataBase/XObjectFactory.h>
#include <lib00_utilty/XUtilty.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <numeric>
#include <highfive/H5File.hpp>

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

	//child1->removeData(child1_1);

	auto listNode =makeShareDbObject<XDataListT<XDataObject>>();
	auto subNode1 = makeShareDbObject<XDataObject>(); subNode1->setName("subNode1");
	auto subNode2 = makeShareDbObject<XDataObject>(); subNode2->setName("subNode2");
	

	listNode->append(subNode1);
	listNode->append(subNode2);

	root->addData(listNode);

	HighFive::File file("test.h5", HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
	root->serialize(file.createGroup(root->getName()));
}

void testXDataObject02(){
	auto attr_bool = makeShareDbObject<XAttr_Bool>();
	auto attr_point = makeShareDbObject<XAttr_Point4f>();
	XQ::print(attr_bool->getClassName());
	XQ::print(attr_point->getClassName());
}

//序列化和反序列化测试pants jewelry
//鞋子
	class Clothes :public XDataObject {
		REGISTER_CLASS_META_DATA(Clothes, XDataObject);
	public:
		void Init() override {
			XDataObject::Init();
			AttrName->setValue("UnKnown");
			XQ_ATTR_ADD_INIT(AttrPrice, 0);
			XQ_ATTR_ADD_INIT(AttrColor, XQ::XColor(0, 0, 0, 255));
			XQ_ATTR_ADD_INIT(AttrSize, 0);
		}
	public:
		csptr<XAttr_Double> AttrPrice;									//价格
		csptr<XAttr_Color> AttrColor;										//颜色
		csptr<XAttr_UInt> AttrSize;											//尺码
};

class Shoe:public Clothes {
	REGISTER_CLASS_META_DATA(Shoe, Clothes);
public:
	void Init() override{
		Clothes::Init();
	}
};

//裤子
class Pants :public Clothes {
	REGISTER_CLASS_META_DATA(Pants, Clothes);
public:
	void Init() override {
		Clothes::Init();
	}
};

//珠宝
class Jewelry :public XDataObject {
	REGISTER_CLASS_META_DATA(Jewelry, XDataObject);
public:
	void Init() override {
		XDataObject::Init();
		AttrName->setValue("Unknown");
		AttrPrice->setValue(0);
	}
public:
	csptr<XAttr_Double> AttrPrice;									//价格
};

class Person :public XDataObject {
	REGISTER_CLASS_META_DATA(Person, XDataObject);
protected:
	Person()=default;
	~Person()=default;
public:
	void Init() override{
		XDataObject::Init();
		XQ_ATTR_ADD(AttrAge);
		XQ_ATTR_ADD(AttrWight);
		XQ_XDATA_ADD(mShoe);
		XQ_XDATA_ADD(mPants);
		XQ_XDATA_ADD(mFriends);
		//XQ_XDATA_ADD(mJewelry);

		AttrName->setValue("Nobody");
		AttrAge->setValue(25);
		AttrWight->setValue(70);
	}
public:
	csptr<XAttr_Int> AttrAge;											//年龄
	csptr<XAttr_Int> AttrWight;										//体重
public:
	csptr<Shoe> mShoe;													//鞋子,一个人一双鞋
	csptr<Pants> mPants;												//裤子,一个人一套裤子
	//csptr<XDataListT<Jewelry>> mJewelry;					//珠宝,一个人一串珠宝
	csptr<XDataListT<Person>> mFriends;					//朋友,一个人有多个朋友
};

sptr<Jewelry> createJewelry(const std::string& name, double price) {
	auto jewelry = makeShareDbObject<Jewelry>();
	jewelry->AttrName->setValue(name);
	jewelry->AttrPrice->setValue(price);
	return jewelry;
}

sptr<Person> createPerson(const std::string& name, int age, int wight) {
	auto person =makeShareDbObject<Person>();
	person->AttrName->setValue(name);
	person->AttrAge->setValue(age);
	person->AttrWight->setValue(wight);
	return person;
}

void testSerialization() {
	//注册的工作
	XBaseObjectMeta::registerObject<Clothes>();
	XBaseObjectMeta::registerObject<Shoe>();
	XBaseObjectMeta::registerObject<Pants>();
	XBaseObjectMeta::registerObject<Jewelry>();
	XBaseObjectMeta::registerObject<Person>();
	XBaseObjectMeta::registerObject<XDataListT<Jewelry>>();
	XBaseObjectMeta::registerObject<XDataListT<Person>>();

	//创建一个人和他的朋友，然后保存
	auto me = createPerson("swj",32,70);
	me->mShoe->AttrName->setValue("NewJean");
	me->mShoe->AttrPrice->setValue(200);
	me->mShoe->AttrColor->setValue(XQ::XColor(255, 0, 0, 255));
	me->mShoe->AttrSize->setValue(42);

	me->mPants->setName("BM");
	me->mPants->AttrPrice->setValue(125);
	me->mPants->AttrSize->setValue(175);

	me->mFriends->setName("friends");
	
	{
		auto wife =makeShareDbObject<Person>();
		wife->AttrName->setValue("XQ");
		wife->mShoe->AttrName->setValue("Shock");
		wife->mShoe->AttrPrice->setValue(260);
		wife->mShoe->AttrColor->setValue(XQ::XColor(255, 0, 0, 255));
		wife->mShoe->AttrSize->setValue(42);

		wife->mPants->setName("BSD");
		wife->mFriends->setName("NoFriends");
		me->mFriends->append(wife);
	}
	

	HighFive::File file("me.h5", HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
	auto ss = me->getName();
	me->serialize(file.createGroup(me->getName()));
}

void testDeserialization() {
	
	//注册的工作
	XBaseObjectMeta::registerObject<Clothes>();
	XBaseObjectMeta::registerObject<Shoe>();
	XBaseObjectMeta::registerObject<Pants>();
	XBaseObjectMeta::registerObject<Jewelry>();
	XBaseObjectMeta::registerObject<Person>();
	XBaseObjectMeta::registerObject<XDataListT<Jewelry>>();
	XBaseObjectMeta::registerObject<XDataListT<Person>>();

	auto me =makeShareDbObject<Person>();
	{
		me->beginBatch();
		HighFive::File file("me.h5", HighFive::File::ReadOnly);
		me->deserialize(file.getGroup("swj"));
		me->endBatch();
	}
	{
		HighFive::File file("meCopy.h5", HighFive::File::ReadWrite | HighFive::File::Create | HighFive::File::Truncate);
		me->serialize(file.createGroup(me->getName()));
	}
	
	int i=0;
}

void testXDataPath() {
	auto createObj = [](const std::string& name) {
		auto obj = makeShareDbObject<XDataObject>();
		obj->setName(name);
		return obj;
		};

	auto t1 = createObj("1");
	auto t2 = createObj("2");
	auto t3 = createObj("3");
	auto t4 = createObj("4");
	auto t5 = createObj("5");
	t1->addData(t2);
	t1->addData(t4);

	t2->addData(t3);
	t4->addData(t5);

	auto path1 =t3->getPathFromThis(t2);
	std::cout << path1.path() << std::endl;

	auto ss = t3->getFromPath(path1);
	std::cout<<ss->getName()<<std::endl;
}

int main() {
	//testXDataObject01();
	//testXDataObject02();
	//testSerialization();
	testDeserialization();
	//testXDataPath();
	return 1;
}