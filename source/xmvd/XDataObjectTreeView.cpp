#include "XDataObjectTreeView.h"
#include "XDataObjectTreeModel.h"
#include "XDataObjectTreeItem.h"
#include <qheaderview.h>
#include <QContextMenuEvent>
#include <QMenu>

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

void XDataObjectTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	auto index = indexAt(event->pos());
	if (!index.isValid())return;

	if (auto item = getItem(currentIndex()))
	{
		if (item)
		{
			QMenu* pop_menu = new QMenu(this);
			item->buildContextMen(pop_menu);
			pop_menu->exec(QCursor::pos());
			event->accept();
		}
	}
}

XDataObjectTreeItem* XDataObjectTreeView::getItem(const QModelIndex& index)
{
	if (auto item = mData->model->getItem(index)) {
		return item;
	}
	return nullptr;
}

void XDataObjectTreeView::slotCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (auto item = mData->model->getItem(current)) {
		auto object = item->getData();
		sigCurrentObjectChanged(object);
	}
}
