#include "XDataObjectTreeModel.h"
#include "XDataObjectTreeItem.h"

class XDataObjectTreeModel::Internal {
	public:
		std::vector<QString> headers{"Node","ClassName"};
};
XDataObjectTreeModel::XDataObjectTreeModel(QObject* parent):mData(new Internal)
{
}

XDataObjectTreeModel::~XDataObjectTreeModel()
{
}


XDataObjectTreeItem* XDataObjectTreeModel::getItem(const QModelIndex& index) const
{
	if (!index.isValid()) {
		
		return m_rootItem;
	}
	
	auto item = static_cast<XDataObjectTreeItem*>(index.internalPointer());
	return item;
}

void XDataObjectTreeModel::setRootItem(XDataObjectTreeItem* rootItem)
{
	m_rootItem = rootItem;
	beginResetModel();
	endResetModel();
}

void XDataObjectTreeModel::updateModel()
{
	beginResetModel();
	endResetModel();
}

QVariant XDataObjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	if (auto* item = getItem(index)) {
		return item->data(index.column(),role);
	}
	
	return QVariant();
}

bool XDataObjectTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

Qt::ItemFlags XDataObjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return  QAbstractItemModel::flags(index);
}

QVariant XDataObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical ) {
		if(role ==Qt::DisplayRole)
			return section;
		return QVariant();
	}
	if (orientation == Qt::Horizontal) {
		if (role == Qt::DisplayRole)
			return mData->headers[section];
		return QVariant();
	}
    return QVariant();
}

QModelIndex XDataObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	XDataObjectTreeItem* parentItem = getItem(parent);
	XDataObjectTreeItem* childItem = parentItem->child(row);

	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex XDataObjectTreeModel::parent(const QModelIndex& index) const
{
	auto* childItem = getItem(index);
	auto* parentItem = childItem->parent();

	if (childItem == m_rootItem || parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->index(), index.column(), parentItem);
}

int XDataObjectTreeModel::rowCount(const QModelIndex& parent) const
{
	if (auto* parentItem = getItem(parent)) {
		return  parentItem->childNum();
	}
	return 0;
}

int XDataObjectTreeModel::columnCount(const QModelIndex& parent) const
{
	if (auto* parentItem = getItem(parent)) {
		return  parentItem->columnCount();
	}
	return 0;
}