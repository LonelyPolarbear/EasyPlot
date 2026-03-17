#include <iostream>
#include <xmvd/XDataObjectTableView.h>
#include <xmvd/XDataObjectTreeView.h>
#include <xmvd/XObjectInspectorView.h>
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
		XQ_XDATA_ADD(child);
	}
public:
	csptr<XAttr_Color> AttrColor;
	csptr<XAttr_Enum<PolygonMode>> AttrEnum;
	csptr<XAttr_Int> AttrInt;
	csptr<XDataObject> child;
};

void testTableView() {
	XDataObjectTableView view;
	view.resize(300, 200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);
}

void testTreeView() {
	XDataObjectTreeView view;
	view.resize(300, 200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);
}

int main(int argc, char** argv) {
	XBaseObjectMeta::registerObject<XAttr_Enum<MyEnum>>();
	XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<MyEnum>>(), XattrEnumToString<MyEnum>);

	QApplication app(argc, argv);


	/*XDataObjectTreeView view;
	view.resize(300, 200);
	view.show();

	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	view.setDataObject(object);


	XDataObjectTableView tableView;
	tableView.resize(300, 200);
	tableView.show();
	tableView.setDataObject(object);

	xsig::connect(&view, &XDataObjectTreeView::sigCurrentObjectChanged, [&](std::shared_ptr<XDataObject> obj) {
		tableView.setDataObject(obj);
		});*/
		

	XObjectInspectorView InspectorView;	

	InspectorView.resize(300, 200);
	InspectorView.show();
	auto object = makeShareDbObject<MyObject>();
	object->setName("test");
	InspectorView.setRootObject(object);

	return app.exec();
}