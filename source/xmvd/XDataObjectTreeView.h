#pragma once
#include "xmvdApi.h"
#include <memory>
#include <qtreeview.h>
class XDataObjectTreeModel;
class XDataObject;
class XMVD_API XDataObjectTreeView : public QTreeView {
	Q_OBJECT
public:
	explicit XDataObjectTreeView(QWidget* parent = nullptr);
	void setDataObject(const std::shared_ptr<XDataObject>& dataObject);
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};