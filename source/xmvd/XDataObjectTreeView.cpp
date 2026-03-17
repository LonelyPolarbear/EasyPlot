#include "XDataObjectTreeView.h"
#include "XDataObjectTreeModel.h"
#include "XDataObjectTreeItem.h"
#include <qheaderview.h>

class XDataObjectTreeView::Internal {
public:
	XDataObjectTreeModel *model =nullptr;
	XDataObjectTreeItem*	rootItem =nullptr;
	sptr<XDataObject> dataObject;
};
XDataObjectTreeView::XDataObjectTreeView(QWidget* parent):mData(new Internal)
{
	
	mData->model = new XDataObjectTreeModel(this);
	this->setModel(mData->model);
	mData->rootItem = new XDataObjectTreeItem(nullptr, nullptr);
	mData->model->setRootItem(mData->rootItem);
	setRootIsDecorated(true);
	this->setAlternatingRowColors(true);

	connect(this->selectionModel(),&QItemSelectionModel::currentRowChanged,this, &XDataObjectTreeView::slotCurrentRowChanged);
}

XDataObjectTreeView::~XDataObjectTreeView()
{
	delete mData->rootItem;
}

void XDataObjectTreeView::setDataObject(const std::shared_ptr<XDataObject>& dataObject)
{
	if (!dataObject) {
		return;
	}

	if(mData->dataObject == dataObject)
		return;
	mData->dataObject = dataObject;
	auto item = XDataObjectTreeItem::InitFromDataObject(dataObject);

	mData->rootItem->clear();
	mData->rootItem->addChild(item);

	mData->model->updateModel();
	expandAll();
	for (int i = 0; i < mData->model->columnCount(); ++i) {
		resizeColumnToContents(i);
	}
}

void XDataObjectTreeView::slotCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (auto item = mData->model->getItem(current)) {
		auto object = item->getData();
		sigCurrentObjectChanged(object);
	}
}
