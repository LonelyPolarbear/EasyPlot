#include "XDataObject.h"
#include "XDataAttribute.h"
#include <atomic>
static std::atomic< uint64_t>  object_id_counter(0);

XDataObject::XDataObject(sptr<XDataObject> parent):mUid(object_id_counter++), mParent(parent)
{
}

XDataObject::~XDataObject()
{
}

bool XDataObject::addData(sptr<XDataObject> data)
{
	if (hasData(data)) {
		return false;
	}
	if (!data->getParent()) {
		data->setParant(asDerived<XDataObject>());
	}
	mChilds.push_back(data);
	return true;
}

bool XDataObject::removeData(sptr<XDataObject> data)
{
	if (!hasData(data)) {
		return false;
	}
	auto iter = std::remove(mChilds.begin(), mChilds.end(), data);
	for (auto i = iter; i != mChilds.end(); ++i) {
		(*iter)->setParant(nullptr);
	}
	mChilds.erase( iter, mChilds.end());
	return true;
}

sptr<XDataObject> XDataObject::getParent() const
{
	return mParent.lock();
}

bool XDataObject::addAttribute(sptr<XDataAttribute> attr)
{
	if (hasAttribute(attr)) {
		return false;
	}
	attr->mParent = asDerived<XDataObject>();
	mAttributes.push_back(attr);
}

bool XDataObject::hasData(sptr<XDataObject> data)
{
	for (auto s : mChilds) {
		if (s == data) {
			return true;
		}
	}
	return false;
}

bool XDataObject::hasAttribute(sptr<XDataAttribute> attr)
{
	for (auto s : mAttributes) {
		if(s == attr)
			return true;
	}
	return false;
}

void XDataObject::setParant(sptr<XDataObject> parent)
{
	mParent = parent;
}

void XDataObject::ItemDataChangedCB(sptr<XDataAttribute> attr, XDataChangeType type)
{
	//调用响应的事件
	slotItemDataChanged(attr,type);
	//发送信号,用于通知外部
	sigItemDataChanged(attr, type);
}

void XDataObject::slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type)
{
	
}
