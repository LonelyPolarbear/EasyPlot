#pragma once
#include "xmvdApi.h"
#include <memory>
#include <vector>

#include <QVariant>

class XDataObject;
class XDataObjectTreeItem {
public:
	explicit XDataObjectTreeItem(std::shared_ptr<XDataObject> data, XDataObjectTreeItem *parent = nullptr);
	~XDataObjectTreeItem();
	//set
	void setParent(XDataObjectTreeItem* parent);
	void setData(std::shared_ptr<XDataObject> data);
	void addChild(XDataObjectTreeItem* child);
	void clear();

	//get
	XDataObjectTreeItem* child(int row) const;
	XDataObjectTreeItem* parent() const;
	int index() const;
	size_t childNum() const;
	QVariant data(int column, int  role) const;
	int columnCount() const;
	std::shared_ptr<XDataObject> getData() const;
	static XDataObjectTreeItem* InitFromDataObject(std::shared_ptr<XDataObject> object);
protected:
	std::vector<XDataObjectTreeItem*> m_childItems;
	XDataObjectTreeItem* m_parentItem;
	std::shared_ptr<XDataObject> m_data;
};