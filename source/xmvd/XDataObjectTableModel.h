#pragma once
#include "xmvdApi.h"
#include <dataBase/XDataAttribute.h>
#include <QAbstractTableModel>
#include <memory>

extern template class XMVD_API classProcessorFactory<1, QString(sptr<XDataAttribute>)>;
using XattrToQstringFactory = classProcessorFactory<1, QString(sptr<XDataAttribute>)>;

class XDataObject;
class XDataAttribute;
class XMVD_API XDataObjectTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	explicit XDataObjectTableModel(QObject* parent = nullptr);
	~XDataObjectTableModel();
public:
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	void setDataObject(std::shared_ptr<XDataObject>);

	std::shared_ptr<XDataAttribute>  getAttr(const QModelIndex& index);
protected:
	std::shared_ptr<XDataObject> getDataObject() const;
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};