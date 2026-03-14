#include "XDataObjectTreeView.h"

class XDataObjectTreeView::Internal {

};
XDataObjectTreeView::XDataObjectTreeView(QWidget* parent):mData(new Internal)
{
}

void XDataObjectTreeView::setDataObject(const std::shared_ptr<XDataObject>& dataObject)
{
}
