#include <iostream>
#include <xmvd/XDataObjectTableView.h>
#include <xmvd/XDataObjectTreeView.h>
#include <xmvd/XObjectInspectorView.h>
#include <qapplication.h>
#include <dataBase/XDataObject.h>
#include <dataBase/XDataAttribute.h>
#include <dataBase/XObjectFactory.h>

#include <lib05_shape/XRenderNodeAttribute.h>
#include <xtest/XTest.h>

int g_argc;
char** g_argv;

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
		XQ_ATTR_ADD_INIT(AttrVec, XQ::Vec3d(1.2, 2.3, 3.4));
		XQ_ATTR_ADD_INIT(AttrInt, 20);
		XQ_XDATA_ADD(child);
		
	}
public:
	csptr<XAttr_Color> AttrColor;
	csptr<XAttr_Enum<PolygonMode>> AttrEnum;
	csptr<XAttr_Int> AttrInt;
	csptr<XAttr_Vec3d> AttrVec;
	csptr<XDataObject> child;
};

void testTableView() {

	QApplication app(g_argc, g_argv);

	XDataObjectTableView view;
	view.resize(300, 200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);

	app.exec();
}

void testTreeView() {
	QApplication app(g_argc, g_argv);
	XDataObjectTreeView view;
	view.resize(300, 200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);
	app.exec();
}

void testXObjectInspectorView() {
	QApplication app(g_argc, g_argv);

	XObjectInspectorView InspectorView;
	InspectorView.resize(300, 200);
	InspectorView.show();
	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	InspectorView.setRootObject(object);

	app.exec();
}


int main(int argc, char** argv) {
	g_argc = argc;
	g_argv = argv;

	XBaseObjectMeta::registerObject<XAttr_Enum<MyEnum>>();
	XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<MyEnum>>(), XattrEnumToString<MyEnum>);

	XTestApp app("XMVD Test");

	app.addCmd("testTableView","表格控件",testTableView);
	app.addCmd("testTreeView","树控件",testTreeView);
	app.addCmd("testXObjectInspectorView","对象属性编辑器",testXObjectInspectorView);

	return app.run();
}