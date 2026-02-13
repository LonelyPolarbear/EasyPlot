#include <iostream>
#include <dataBase/XVector.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataArray.h>
#include <xalgo/XAlgo.h>
#include <lib00_utilty/XUtilty.h>
#include <numeric>


void test_lerp_rgb() {
	XQ::print("lerp_rgb...");

	XQ::XColor c1(255, 255, 255, 255);
	XQ::XColor c2(255, 255, 255, 255);
	float t = 0.2;
	auto out = XQ::XAlgo::lerp_rgb(c1, c2, t);

	XQ::print("c1:", c1, " c2:", c2, " out:", out);
}

void test_combineArray() {
	XQ::print("combineArray...");
	auto t1 =makeShareDbObject<XIntArray>();
	t1->setComponent(3);
	t1->setNumOfTuple(5);
	for (int i = 0; i < 5; i++) {
		t1->setTuple(i,10+i,20+i,30+i);
	}

	auto t2 = makeShareDbObject<XIntArray>();
	t2->setComponent(3);
	t2->setNumOfTuple(5);
	for (int i = 0; i < 5; i++) {
		t2->setTuple(i, 40 + i, 50 + i, 60 + i);
	}

	auto t3 = XQ::XAlgo::combineArray(std::vector<sptr<XIntArray>> {t1,t2});
	t1->dump();
	t2->dump();
	t3->dump();

	auto t4 = makeShareDbObject<XIntArray>();
	t4->setComponent(3);

	XQ::XAlgo::combineArray(std::vector<sptr<XIntArray>> {t1, t2},t4);
	t4->dump();
}

void test_linespace() {
	auto out =XQ::XAlgo::linspace<double>(1,10,6);
	//¥Ú”°
	std::copy(out.begin(), out.end(), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;
}

int main() {
	//test_combineArray();
	test_linespace();
	return 1;
}