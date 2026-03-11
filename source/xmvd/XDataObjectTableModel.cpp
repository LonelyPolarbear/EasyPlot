#include "XDataObjectTableModel.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XObjectFactory.h>
#include <QVector>
#include <QColor>

template class XMVD_API classProcessorFactory<1, QString(sptr<XDataAttribute>)>;

class XDataObjectTableModel::Internal {
public:
	wptr<XDataObject> data;

	QVector<QString> headers={"Attribute","Value"};

	void analyzeData() {
		//获取继承关系
		auto obj = data.lock();
		if(!obj)
			return;
		std::string name = obj->getClassName();
		std::vector<std::string> inheritChain;
		
		while (name.size()>0)
		{
			inheritChain.insert(inheritChain.begin(), name);
			auto parents = XBaseObjectMeta::GetParents(obj->getClassName());
			if (parents.size() > 0) {
				auto curParent = parents[0];
				if (curParent == "XDataObject") {
					//到达最顶层父类,不再往上查找
					name = "";
				}
				else {
					name = curParent;
				}
			}
			else {
				name ="";
			}
		}

		//拿到了继承关系链条
		
	}

	bool isBoolAttr(sptr<XDataAttribute> attr) {
		return attr->getClassName() == XQ_META::ClassName<XAttr_Bool>();
	}

	bool isColorAttr(sptr<XDataAttribute> attr) {
		return attr->getClassName() == XQ_META::ClassName<XAttr_Color>();
	}

	sptr<XDataAttribute>  getAttr(const QModelIndex& index) {
		auto obj = data.lock();
		if (!obj)
			return nullptr;
		return obj->attrAt(index.row());
	}
};

XDataObjectTableModel::XDataObjectTableModel(QObject* parent):mData(new Internal)
{
}

XDataObjectTableModel::~XDataObjectTableModel()
{
}

int XDataObjectTableModel::rowCount(const QModelIndex& parent) const
{
	auto obj =getDataObject();
	if(!obj)
		return 0;
	return obj->attrCount();
}

int XDataObjectTableModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

QVariant XDataObjectTableModel::data(const QModelIndex& index, int role) const
{
	auto row = index.row();
	auto col = index.column();
	
	auto attr = mData->getAttr(index);
	if(!attr)
		return QVariant();

	auto className = attr->getClassName();

	if (col == 0) {
		if (role == Qt::DisplayRole) {
			auto str = QString::fromStdString(attr->getName());
			return QString::fromStdString(attr->getName());
		}
		return QVariant();
	}

	if (col == 1) {
		if (role == Qt::DisplayRole) {
			if (mData->isBoolAttr(attr) || mData->isColorAttr(attr)) {
				return QVariant();
			}
			else {
				if (XattrToQstringFactory::instance().hasProcessor(className)) {
					return XattrToQstringFactory::instance().process(className, attr);
				}
				return "unknown";
			}
			
		}
		if (role == Qt::CheckStateRole) {
			if (mData->isBoolAttr(attr)) {
				auto flag = attr->asDerived<XAttr_Bool>();
				if (flag->getValue()) {
					return Qt::Checked;
				}
				else {
					return Qt::Unchecked;
				}
			}
			return QVariant();
		}

		if (role == Qt::BackgroundColorRole) {
			if (mData->isColorAttr(attr)) {
				auto color = attr->asDerived<XAttr_Color>()->getValue();
				return QColor(color.r(), color.g(), color.b(), color.a());
			}
			return QVariant();
		}
	}
	
	return QVariant();
}

Qt::ItemFlags XDataObjectTableModel::flags(const QModelIndex& index) const
{
	auto row = index.row();
	auto col = index.column();

	auto attr =mData->getAttr(index);
	if(!attr)
		return QAbstractTableModel::flags(index);
	if (col == 1) {
		if(mData->isBoolAttr(attr))
			return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
		else
			return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	}

	return QAbstractTableModel::flags(index);
}

QVariant XDataObjectTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return mData->headers[section];
	}
	return QVariant();
}

bool XDataObjectTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	auto row = index.row();
	auto col = index.column();
	auto attr = mData->getAttr(index);
	if (!attr)
		return false;

	if (col == 1) {
		if (role == Qt::CheckStateRole && mData->isBoolAttr(attr) ) {
			auto boolAttr =attr->asDerived<XAttr_Bool>();
			boolAttr->setValue(!boolAttr->getValue());
			emit dataChanged(index, index);
			return true;
		}
		else if (role ==Qt::EditRole && mData->isColorAttr(attr)) {
			auto colorAttr = attr->asDerived<XAttr_Color>();
			QColor c =value.value<QColor>();
			colorAttr->setValue(XQ::XColor(c.red(), c.green(), c.blue(), c.alpha()));
			emit dataChanged(index, index);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void XDataObjectTableModel::setDataObject(std::shared_ptr<XDataObject> obj)
{
	mData->data = obj;
	beginResetModel();
	endResetModel();
}

std::shared_ptr<XDataAttribute> XDataObjectTableModel::getAttr(const QModelIndex& index)
{
	return mData->getAttr(index);
}

std::shared_ptr<XDataObject> XDataObjectTableModel::getDataObject() const
{
	return mData->data.lock();
}
