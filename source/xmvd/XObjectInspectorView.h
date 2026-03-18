#pragma once
#include "xmvdApi.h"
#include <memory>
#include <QWidget>
#include <QSplitter>
#include "xsignal/XSignal.h"

class XDataObject;
class XDataObjectTreeView;
class XDataObjectTableView;
class XMVD_API XObjectInspectorView :public QWidget {
	Q_OBJECT
public:
	explicit XObjectInspectorView(QWidget* parent = nullptr);

	// 设置要检视的根对象
	void setRootObject(std::shared_ptr<XDataObject> root);

	virtual void bindSignal();
protected:
	void BuildUI();
protected:
	XDataObjectTreeView* m_treeView = nullptr;
	XDataObjectTableView* m_propertyView = nullptr;
	QSplitter* m_splitter = nullptr;

	class Internal;
	std::shared_ptr<Internal> mData;
};