#include "XDataObject.h"
 
#include <atomic>
#include<string>
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
	sigDataAdd(data);
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
	sigDataRemove(data);
	return true;
}

sptr<XDataObject> XDataObject::getParent() const
{
	return mParent.lock();
}

void XDataObject::Init()
{
	XDataBaseObject::Init();
	XQ_ATTR_ADD_INIT(AttrVisible, true);
	XQ_ATTR_ADD_INIT(AttrName, std::to_string(mUid));
	xsig::connect(this, &XDataObject::sigDataChanged, this, &XDataObject::DataChangedCB);
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

void XDataObject::slotDataChanged(sptr<XDataObject>, XDataChangeType)
{
}

sptr<XDataAttribute> XDataObject::getAttribute(const std::string& name) const
{
	for (auto s : mAttributes) {
		if (s->getName() == name)
			return s;
	}
	return nullptr;
}

void XDataObject::ItemDataChangedCB(sptr<XDataAttribute> attr, XDataChangeType type)
{
	//调用响应的事件
	slotItemDataChanged(attr,type);
	//发送信号,用于通知外部
	sigItemDataChanged(attr, type);

	std::vector<std::string> path;
	ChildItemDataChanged(path, attr, type);
}

void XDataObject::DataChangedCB(sptr<XDataObject> obj, XDataChangeType type)
{
	slotDataChanged(obj, type);
	
	std::vector<std::string> path;
	ChildDataChanged(path, obj, type);
}

void XDataObject::ChildItemDataChanged(std::vector<std::string>& path, sptr<XDataAttribute> attr, XDataChangeType type) {
	//发射信号
	if (path.size() != 0)
		sigChildItemDataChanged(path, attr, type);
	//通知父类
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.push_back(AttrName->getValue());
			parent->ChildItemDataChanged(path, attr, type);
		}
	}
}

void XDataObject::ChildDataChanged(std::vector<std::string>& path, sptr<XDataObject> obj, XDataChangeType type) {
	//如果path的大小是1,说明是当前obj，不需要发射信号，直接向上传递即可
	if(path.size() != 0)
		sigChildDataChanged(path, obj, type);
	//通知父类
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.push_back(AttrName->getValue());
			parent->ChildDataChanged(path, obj, type);
		}
	}
}

void XDataObject::slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type)
{
	
}
