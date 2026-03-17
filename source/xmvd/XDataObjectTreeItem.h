#pragma once
#include "xmvdApi.h"
#include <memory>
#include <vector>

#include <QVariant>
#include <xsignal/XSignal.h>

class QMenu;
class XDataObject;
class XDataObjectTreeItem {
public:
	explicit XDataObjectTreeItem(std::shared_ptr<XDataObject> data, XDataObjectTreeItem *parent = nullptr);
	~XDataObjectTreeItem();
	//set
	void setParent(XDataObjectTreeItem* parent);
	void setData(std::shared_ptr<XDataObject> data);
	void addChild(XDataObjectTreeItem* child);
	void removeChild(XDataObjectTreeItem* child);
	void clear();

	XDataObjectTreeItem *getRoot() const;
	//get
	XDataObjectTreeItem* child(int row) const;
	XDataObjectTreeItem* parent() const;
	int index() const;
	size_t childNum() const;
	QVariant data(int column, int  role) const;
	int columnCount() const;
	std::shared_ptr<XDataObject> getData() const;
	static XDataObjectTreeItem* InitFromDataObject(std::shared_ptr<XDataObject> object, XDataObjectTreeItem* item =nullptr);


	void buildContextMen(QMenu*);
public:
	XSIGNAL(void(XDataObjectTreeItem* parent,XDataObjectTreeItem* cur)) sigDataAdd;
	XSIGNAL(void(XDataObjectTreeItem*)) sigDataRemove;
protected:
	std::vector<XDataObjectTreeItem*> m_childItems;
	XDataObjectTreeItem* m_parentItem;
	std::shared_ptr<XDataObject> m_data;
	class Internal;
	std::shared_ptr<Internal> mData;
};