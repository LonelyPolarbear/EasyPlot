#pragma once
#include "xmvdApi.h"
#include <dataBase/XDataAttribute.h>
#include <QAbstractTableModel>
#include <memory>
class XDataObjectTreeItem;
class XDataObject;
class XMVD_API XDataObjectTreeModel : public QAbstractItemModel {
	Q_OBJECT
public:
	explicit XDataObjectTreeModel(QObject* parent = nullptr);
	~XDataObjectTreeModel();
public:
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	XDataObjectTreeItem* getItem(const QModelIndex& index) const;
	void setRootItem(XDataObjectTreeItem* rootItem);
	void updateModel();

	QModelIndex XDataObjectTreeModel::indexFromItem(XDataObjectTreeItem* item) const;

	bool removeItem(XDataObjectTreeItem* item);

	bool insertItem(XDataObjectTreeItem* parentItem,  XDataObjectTreeItem* item,int row);

	void slotRemoveItem(XDataObjectTreeItem* item);

	void slotAddItem(XDataObjectTreeItem* parentItem, XDataObjectTreeItem* item);
protected:
	XDataObjectTreeItem* m_rootItem=nullptr;
	class Internal;
	std::shared_ptr<Internal> mData;
};