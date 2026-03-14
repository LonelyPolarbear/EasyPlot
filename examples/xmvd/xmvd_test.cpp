#include <iostream>
#include <xmvd/XDataObjectTableView.h>
#include <qapplication.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <dataBase/XObjectFactory.h>

#include <lib05_shape/XRenderNodeAttribute.h>

enum class MyEnum {
	point,
	line
};

class MyObject :public XDataObject {
	REGISTER_CLASS_META_DATA(MyObject, XDataObject);
public:
	void Init() override {
		XDataObject::Init();
		XQ_ATTR_ADD_INIT(AttrColor, XQ::XColor(255, 0, 255, 255));
		XQ_ATTR_ADD_INIT(AttrEnum, PolygonMode::face);
		XQ_ATTR_ADD_INIT(AttrInt, 20);
	}
public:
	csptr<XAttr_Color> AttrColor;
	csptr<XAttr_Enum<PolygonMode>> AttrEnum;
	csptr<XAttr_Int> AttrInt;
};



int main(int argc,char **argv) {
	XBaseObjectMeta::registerObject<XAttr_Enum<MyEnum>>();
	XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<MyEnum>>(), XattrEnumToString<MyEnum>);

	QApplication app(argc, argv);
	XDataObjectTableView view;
	view.resize(300,200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);
	return app.exec();
}