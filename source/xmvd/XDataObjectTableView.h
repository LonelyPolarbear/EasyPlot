#pragma once
#include "xmvdApi.h"
#include <memory>
#include <QTableView>
class XDataObjectTableModel;
class XDataObject;
class XMVD_API XDataObjectTableView : public QTableView {
	Q_OBJECT
public:
	explicit XDataObjectTableView(QWidget* parent = nullptr);
	void setDataObject(const std::shared_ptr<XDataObject>& dataObject);
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};