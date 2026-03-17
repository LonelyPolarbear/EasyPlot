#pragma once
#include "xmvdApi.h"
#include <memory>
#include <qtreeview.h>
#include "xsignal/XSignal.h"
class XDataObjectTreeModel;
class XDataObjectTreeItem;
class XDataObject;
class XMVD_API XDataObjectTreeView : public QTreeView {
	Q_OBJECT
public:
	explicit XDataObjectTreeView(QWidget* parent = nullptr);
	~XDataObjectTreeView();
	void setDataObject(const std::shared_ptr<XDataObject>& dataObject);
public Q_SLOTS:
	void slotCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);

protected:
	void contextMenuEvent(QContextMenuEvent* event)override;

	XDataObjectTreeItem* getItem(const QModelIndex& index);
public:
	XSIGNAL(void(std::shared_ptr<XDataObject>)) sigCurrentObjectChanged;
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};