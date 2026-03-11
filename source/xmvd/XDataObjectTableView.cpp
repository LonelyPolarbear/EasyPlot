#include "XDataObjectTableView.h"
#include "XDataObjectTableModel.h"
#include "XAttrItemDelegate.h"
#include <QHeaderView>

class XDataObjectTableView::Internal {
public:
	XDataObjectTableModel *model;
};

XDataObjectTableView::XDataObjectTableView(QWidget* parent):mData(new Internal)
{
	mData->model = new XDataObjectTableModel(this);
	this->setModel(mData->model);
	//½çÃæÉèÖÃ

	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	this->setShowGrid(true);
	this->setAlternatingRowColors(true);
	this->horizontalHeader()->setStretchLastSection(true);
	auto editDelegate = new XAttrItemDelegate(this);
	this->setItemDelegateForColumn(1, editDelegate);
}

void XDataObjectTableView::setDataObject(const std::shared_ptr<XDataObject>& dataObject)
{
	mData->model->setDataObject(dataObject);
}
