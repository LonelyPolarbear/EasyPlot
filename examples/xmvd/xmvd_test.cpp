#include <iostream>
#include <xmvd/XDataObjectTableView.h>
#include <qapplication.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>

class MyObject :public XDataObject {
	REGISTER_CLASS_META_DATA(MyObject, XDataObject);
public:
	void Init() override {
		XDataObject::Init();
		XQ_ATTR_ADD_INIT(AttrColor, XQ::XColor(255,0,255,255));
	}
public:
	csptr<XAttr_Color> AttrColor;
};

int main(int argc,char **argv) {
	QApplication app(argc, argv);
	XDataObjectTableView view;
	view.resize(300,200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);
	return app.exec();
}