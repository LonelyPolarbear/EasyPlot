#include "XDataObjectTableModel.h"
#include <dataBase/XDataObject.h>
#include <dataBase/XObjectFactory.h>
#include <QVector>
#include <QColor>

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

	bool isEnumAttr(sptr<XDataAttribute> attr) {
		return XBaseObjectMeta::IsA(attr->getClassName(), XQ_META::ClassName<XDataAttributeEnumBase>());
	}

	bool isStrAttr(sptr<XDataAttribute> attr) {
		return attr->getClassName() == XQ_META::ClassName<XAttr_String>();
	}

	bool isDigitAttr(sptr<XDataAttribute> attr,int & type) {
		auto className = attr->getClassName();
		
		if (className == XQ_META::ClassName<XAttr_Int>()) {
			type = 1;
			return true;
		}
		else if (className == XQ_META::ClassName<XAttr_UInt>()) {
			type = 2;
			return true;
		}
		else if (className == XQ_META::ClassName<XAttr_Float>()) {
			type = 3;
			return true;
		}
		else if (className == XQ_META::ClassName<XAttr_Double>()) {
			type = 4;
			return true;
		}
		return false;
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
					return QString::fromStdString(XattrToQstringFactory::instance().process(className, attr));
				}
				return QString::fromStdString(className);
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
		else if (role ==Qt::EditRole) {
			if (mData->isColorAttr(attr)) {
				auto colorAttr = attr->asDerived<XAttr_Color>();
				QColor c = value.value<QColor>();
				colorAttr->setValue(XQ::XColor(c.red(), c.green(), c.blue(), c.alpha()));
				emit dataChanged(index, index);
				return true;
			}
			if (mData->isEnumAttr(attr)) {
				auto enumAttr = attr->asDerived<XDataAttributeEnumBase>();
				enumAttr->setIntValue(value.toInt());
				emit dataChanged(index, index);
				return true;
			}
			if (mData->isStrAttr(attr)) {
				auto strAttr = attr->asDerived<XAttr_String>();
				strAttr->setValue(value.toString().toStdString());
				emit dataChanged(index, index);
				return true;
			}
			int type = 0;
			if (mData->isDigitAttr(attr,type)) {
				value.toInt();
				value.toFloat();
				value.toDouble();
				if (type == 1) {
					attr->asDerived<XAttr_Int>()->setValue(value.toInt());
				}
				else if (type == 2) {
					attr->asDerived<XAttr_UInt>()->setValue(value.toInt());
				}
				else if (type == 3) {
					attr->asDerived<XAttr_Float>()->setValue(value.toFloat());
				}
				else if (type == 4) {
					attr->asDerived<XAttr_Double>()->setValue(value.toDouble());
				}
				return true;
			}
			return false;
		}
		else {
			return false;
		}
	}
	return false;
}

void XDataObjectTableModel::setDataObject(std::shared_ptr<XDataObject> obj)
{
	if(mData->data.lock() == obj)
		return;
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
