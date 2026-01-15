#include <iostream>
#include <dataBase/XVector.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataArray.h>
#include <xalgo/XAlgo.h>
#include <lib00_utilty/XUtilty.h>
#include <numeric>

int main() {
	XQ::print("lerp_rgb...");

	XQ::XColor c1(255, 255, 255, 255);
	XQ::XColor c2(255, 255, 255, 255);
	float t =0.2;
	auto out =XQ::XAlgo::lerp_rgb(c1,c2,t);

	XQ::print("c1:",c1," c2:",c2," out:",out);
	return 1;
}