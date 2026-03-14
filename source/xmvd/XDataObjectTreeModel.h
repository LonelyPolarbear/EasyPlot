#pragma once
#include "xmvdApi.h"
#include <dataBase/XDataAttribute.h>
#include <QAbstractTableModel>
#include <memory>

class XDataObject;
class XMVD_API XDataObjectTreeModel : public QAbstractTableModel {
	Q_OBJECT
public:
	explicit XDataObjectTreeModel(QObject* parent = nullptr);
	~XDataObjectTreeModel();
public:
};