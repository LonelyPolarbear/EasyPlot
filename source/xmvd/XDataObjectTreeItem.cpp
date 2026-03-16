#include "XDataObjectTreeItem.h"
#include <dataBase/XDataObject.h>

void createTreeItemFromObject(XDataObjectTreeItem* parent, std::shared_ptr<XDataObject> object) {
	auto num = object->childCount();
	for (int i = 0; i < num; i++) {
		auto obj = object->childAt(i);
		auto curItem = new XDataObjectTreeItem(obj, parent);
		parent->addChild(curItem);
		createTreeItemFromObject(curItem, obj);
	}
}

XDataObjectTreeItem::XDataObjectTreeItem(std::shared_ptr<XDataObject> data, XDataObjectTreeItem* parent):m_parentItem(parent)
{
    setData(data);
}

XDataObjectTreeItem::~XDataObjectTreeItem()
{
    //递归删除
    for (auto item : m_childItems) {
        delete item;
    }
}

XDataObjectTreeItem* XDataObjectTreeItem::child(int row) const
{
    if(row>=0 && row<m_childItems.size())
        return m_childItems[row];
    return nullptr;
}

XDataObjectTreeItem* XDataObjectTreeItem::parent() const
{
    return m_parentItem;
}

int XDataObjectTreeItem::index() const
{
    //获取当前位置在父节点的子节点中的位置
    if (m_parentItem) {
       auto num = m_parentItem->childNum();
       for (int i = 0; i < num; i++) {
           if (m_parentItem->child(i) == this) {
                return i;
           }
       }
    }
    //如果当前节点没有父节点，则返回-1
    return -1;
}

void XDataObjectTreeItem::setParent(XDataObjectTreeItem* parent)
{
    m_parentItem = parent;
}

void XDataObjectTreeItem::setData(std::shared_ptr<XDataObject> data)
{
    m_data = data;
	//绑定信号
	//data->sigDataAdd
	//sigDataRemove
}

void XDataObjectTreeItem::addChild(XDataObjectTreeItem* childItem)
{
    if(!childItem)
        return;
    m_childItems.push_back(childItem);
    childItem->setParent(this);
}

void XDataObjectTreeItem::clear()
{
    m_childItems.clear();
}

size_t XDataObjectTreeItem::childNum() const
{
    return m_childItems.size();
}

QVariant XDataObjectTreeItem::data( int column,int  role) const
{
    if (role == Qt::DisplayRole) {
        if(column ==0)
            return QString::fromStdString(m_data->getName());
        else if(column ==1)
            return QString::fromStdString(m_data->getClassName());
        else
            return QVariant();
    }
    else {
        return QVariant();
    }
}

int XDataObjectTreeItem::columnCount() const
{
    if (m_data) {
        return 2;
    }
    return 2;
}

std::shared_ptr<XDataObject> XDataObjectTreeItem::getData() const
{
    return m_data;
}


/*
void createTreeItemFromObject(XDataObjectTreeItem* inputItem, std::shared_ptr<XDataObject> object) {
	XDataObjectTreeItem* curItem = new XDataObjectTreeItem(object, inputItem);
	inputItem->addChild(curItem);
	auto num = object->childCount();
	for (int i = 0; i < num; i++) {
		auto obj = object->childAt(i);
		//auto curChildItem = new XDataObjectTreeItem(obj, curItem);
		//parent->addChild(curChildItem);
		createTreeItemFromObject(curItem, obj);
	}
}
XDataObjectTreeItem* XDataObjectTreeItem::InitFromDataObject(std::shared_ptr<XDataObject> object)
{
	XDataObjectTreeItem* parent = new XDataObjectTreeItem(nullptr, nullptr);
	createTreeItemFromObject(parent, object);
	return parent;
}
*/

XDataObjectTreeItem* XDataObjectTreeItem::InitFromDataObject(std::shared_ptr<XDataObject> object)
{
    XDataObjectTreeItem* parent = new XDataObjectTreeItem(object, nullptr);
    createTreeItemFromObject(parent, object);
    return parent;
}
