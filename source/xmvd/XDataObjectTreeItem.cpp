#include "XDataObjectTreeItem.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XDataList.h>
#include <dataBase/XObjectFactory.h>
#include <xsignal/XSignal.h>
#include <QMenu>

class XDataObjectTreeItem::Internal {
public:
	xsig::xconnector connector;
	~Internal() {
		connector.disconnect();
	}
};

XDataObjectTreeItem::XDataObjectTreeItem(std::shared_ptr<XDataObject> data, XDataObjectTreeItem* parent):m_parentItem(parent),mData(new Internal)
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
    mData->connector.connect(data,&XDataObject::sigDataAdd,[this](sptr<XDataObject> subData){
        auto item =InitFromDataObject( subData,this);
        auto root = getRoot();
        root->sigDataAdd(this,item);
    });

	mData->connector.connect(data, &XDataObject::sigDataRemove, [this](sptr<XDataObject> subData) {
        auto root = getRoot();
        for (int i = 0; i < childNum(); i++) {
            auto item = child(i);
            if (item->getData() == subData) {
                root->sigDataRemove(item);
            }
        }
		});
}

void XDataObjectTreeItem::addChild(XDataObjectTreeItem* childItem)
{
    if(!childItem)
        return;
    m_childItems.push_back(childItem);
    childItem->setParent(this);
}

void XDataObjectTreeItem::removeChild(XDataObjectTreeItem* item)
{
    for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
        if (*it == item) {
            m_childItems.erase(it);
            return;
        }
    }
}

void XDataObjectTreeItem::clear()
{
    m_childItems.clear();
}

XDataObjectTreeItem* XDataObjectTreeItem::getRoot() const
{
    auto p = this;
    while (p->parent()) {
        p = p->parent();
    }
    return const_cast<XDataObjectTreeItem*>(p);
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

void createTreeItemFromObject(XDataObjectTreeItem* parent, std::shared_ptr<XDataObject> object) {
	auto num = object->childCount();
	for (int i = 0; i < num; i++) {
		auto obj = object->childAt(i);
		auto curItem = new XDataObjectTreeItem(obj, parent);
		parent->addChild(curItem);
		createTreeItemFromObject(curItem, obj);
	}
}

XDataObjectTreeItem* XDataObjectTreeItem::InitFromDataObject(std::shared_ptr<XDataObject> object, XDataObjectTreeItem *item )
{
    XDataObjectTreeItem* parent = new XDataObjectTreeItem(object, item);
    createTreeItemFromObject(parent, object);
    return parent;
}

void XDataObjectTreeItem::buildContextMen(QMenu* menu)
{
	auto dataobject = getData();
	auto parent = dataobject->getParent();
	if (parent) {
		if (XBaseObjectMeta::IsA(parent->getClassName(), XQ_META::ClassName<XDataList>())) {
			auto action = menu->addAction("delete");
			QObject::connect(action, &QAction::triggered, [p = parent,d = dataobject]() {
				p->asDerived<XDataList>()->remove(d);
				});
		}
	}
	else {
		//do nth
	}
}
