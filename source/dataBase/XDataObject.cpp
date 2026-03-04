#include "XDataObject.h"
 
#include <atomic>
#include<string>
#include <highfive/H5File.hpp>
#include "lib00_utilty/XUtilty.h"
static std::atomic< uint64_t>  object_id_counter(0);

XDataObject::XDataObject(sptr<XDataObject> parent):mUid(object_id_counter++), mParent(parent)
{
	setBubble(true);
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
		data->setParent(asDerived<XDataObject>());
	}
	else {
		if(data->getParent() != asDerived<XDataObject>())
			return false;
	}
	mChilds.push_back(data);
	//需要保证data的名称的唯一性
	data->setName(data->getName());
	DataAddCB(data);
	return true;
}

bool XDataObject::removeData(sptr<XDataObject> data)
{
	if (!hasData(data)) {
		return false;
	}
	for (auto it = mChilds.begin(); it != mChilds.end(); ) {
		if (*it == data) {
			(*it)->setParent(nullptr);
			it = mChilds.erase(it);
		}
		else {
			++it;
		}
	}
	DataRemoveCB(data);
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
	XQ_ATTR_ADD_INIT(AttrName, getClassName()+"_"+std::to_string(mUid));
	xsig::connect(this, &XDataObject::sigDataChanged, this, &XDataObject::DataChangedCB);
}

void XDataObject::setBubble(bool flag)
{
	mEnableBubble = flag;
}

bool XDataObject::getBubble() const
{
	return mEnableBubble;
}

static std::string makeUnique(const std::vector<std::string>& existing, const std::string& base) {
	if (std::find(existing.begin(), existing.end(), base) == existing.end())
		return base;

	int suffix = 1;
	while (true) {
		std::string candidate = base + "_" + std::to_string(suffix);
		if (std::find(existing.begin(), existing.end(), candidate) == existing.end())
			return candidate;
		++suffix;
	}
}

void XDataObject::setName(const std::string& name)
{
	//需要保证name唯一
	std::vector<std::string> exitsName;
	if (auto parent = getParent()) {
		for (auto m : parent->mChilds) {
			if (m != asDerived<XDataObject>()) {
				exitsName.push_back(m->getName());
			}
		}
	}
	
	auto newName = makeUnique(exitsName, name.size()==0 ? getClassName() + "_" + std::to_string(mUid):name);
	AttrName->setValue(newName);
}

std::string XDataObject::getName() const
{
	return AttrName->getValue();
}

void XDataObject::serialize(HighFive::Group& group/*当前组*/)
{
	auto attrNum = attrCount();
	group.createAttribute("className",getClassName());
	for (int i = 0; i < attrNum; i++) {
		auto attr = attrAt(i);
		//写入属性
		//auto attrGroup =group.createGroup(attr->getName());
		attr->serialize(group);
	}

	auto dataGroup = group.createGroup("__data__");
	serializeData(dataGroup);

	auto childNum = childCount();
	for (int i = 0; i<childNum; i++) {
		auto child = childAt(i);
		auto subgroup =group.createGroup(child->getName());
		child->serialize(subgroup);
	}
	group.getFile().flush();
}

void XDataObject::serializeData(HighFive::Group& Datagroup)
{
	Datagroup.createDataSet("mUid", mUid);
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

void XDataObject::setParent(sptr<XDataObject> parent)
{
	mParent = parent;
}

void XDataObject::slotDataChanged(sptr<XDataObject>, XDataChangeType)
{
}

void XDataObject::beginBatch(bool propagateToChildren)
{
	if (m_batchLevel++ == 0) {
		sigDataChanged(asDerived<XDataObject>(), XDataChangeType::BatchBegin);
		for (auto m : mChilds) {
			m->beginBatch(propagateToChildren);
		}
	}
}

void XDataObject::endBatch(bool propagateToChildren)
{
	if (--m_batchLevel == 0) {
		//说明批处理结束
		if (m_hasPendingChanges) {
			//发射信号
			m_hasPendingChanges = false;
			sigDataChanged(asDerived<XDataObject>(), XDataChangeType::BatchEnd);
		}
		for (auto child : mChilds) {
			child->endBatch(propagateToChildren);
		}
	}
}

bool XDataObject::isInBatch() const
{
	return m_batchLevel >0;
}

int XDataObject::childCount() const
{
	return mChilds.size();
}

sptr<XDataObject> XDataObject::childAt(int index) const
{
	return mChilds[index];
}

int XDataObject::attrCount() const
{
	return mAttributes.size();
}

sptr<XDataAttribute> XDataObject::attrAt(int index) const
{
	return mAttributes[index];
}

void XDataObject::setBatchLevel(int level)
{
	m_batchLevel = level;
}

void XDataObject::notify_sigDataAdd(sptr<XDataObject> data)
{
	XQ::print(AttrName->getValue(), "addData", data->AttrName->getValue());
	sigDataAdd(data);
}

void XDataObject::notify_sigDataAdd(const XQ::XDataPath& path, sptr<XDataObject> data)
{
	auto completePath = path;
	completePath.insert(AttrName->getValue());
	completePath.push(data->AttrName->getValue());
	XQ::print("childAddData", "path:", completePath.path());
	sigChildDataAdd(path,data);
}

void XDataObject::notify_sigDataRemove(sptr<XDataObject> data)
{
	XQ::print(AttrName->getValue(), "removeData", data->AttrName->getValue());
	sigDataRemove(data);
}

void XDataObject::notify_sigDataRemove(const XQ::XDataPath& path, sptr<XDataObject> data)
{
	auto completePath = path;
	completePath.insert(AttrName->getValue());
	completePath.push(data->AttrName->getValue());
	XQ::print("childRemoveData", "path:", completePath.path());
	sigChildDataRemove(path, data);
}

void XDataObject::DataAddCB(sptr<XDataObject> data)
{
	if (isInBatch()) {
		m_hasPendingChanges = true;
		data->setBatchLevel(m_batchLevel);
	}
	else {
		notify_sigDataAdd(data);

		XQ::XDataPath path;
		ChildDataAddChanged(path, data);
	}
}

void XDataObject::DataRemoveCB(sptr<XDataObject> data)
{
	if (isInBatch()) {
		m_hasPendingChanges = true;
		data->endBatch();
	}
	else {
		notify_sigDataRemove(data);
		XQ::XDataPath path;
		ChildDataRemoveChanged(path, data);
	}
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
	if (isInBatch()) {
		m_hasPendingChanges = true;
	}
	else {
		//如果不在批处理中
		slotItemDataChanged(attr, type);
		//发送信号,用于通知外部
		sigItemDataChanged(attr, type);

		XQ::XDataPath path;
		ChildItemDataChanged(path, attr, type);
	}
}

void XDataObject::DataChangedCB(sptr<XDataObject> obj, XDataChangeType type)
{
	slotDataChanged(obj, type);
	
	XQ::XDataPath path;
	ChildDataChanged(path, obj, type);
}

void XDataObject::ChildItemDataChanged(XQ::XDataPath& path, sptr<XDataAttribute> attr, XDataChangeType type) {
	//发射信号
	if (path.size() != 0) {
		auto completePath = path;
		completePath.insert(AttrName->getValue());
		XQ::print("ChildAttributeChanged","path:",completePath.path(),"type:", XQ_META::enum_to_string(type));
		sigChildItemDataChanged(path, attr, type);
	}
	//通知父类
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.insert(AttrName->getValue());
			parent->ChildItemDataChanged(path, attr, type);
		}
	}
}

void XDataObject::ChildDataChanged(XQ::XDataPath& path, sptr<XDataObject> obj, XDataChangeType type) {
	//如果path的大小是1,说明是当前obj，不需要发射信号，直接向上传递即可
	if (path.size() != 0) {
		auto completePath = path;
		completePath.insert(AttrName->getValue());
		XQ::print("ChildDataChanged", "path:", completePath.path(), "type:", XQ_META::enum_to_string(type));
		sigChildDataChanged(path, obj, type);
	}
	//通知父类
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.insert(AttrName->getValue());
			parent->ChildDataChanged(path, obj, type);
		}
	}
}

void XDataObject::ChildDataAddChanged(XQ::XDataPath& path, sptr<XDataObject> obj)
{
	if (path.size() != 0)
		notify_sigDataAdd(path,obj);
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.insert(AttrName->getValue());
			parent->ChildDataAddChanged(path, obj);
		}
	}
}

void XDataObject::ChildDataRemoveChanged(XQ::XDataPath& path, sptr<XDataObject> obj)
{
	if (path.size() != 0)
		notify_sigDataRemove(path, obj);
	if (mEnableBubble) {
		auto parent = getParent();
		if (parent) {
			path.insert(AttrName->getValue());
			parent->ChildDataRemoveChanged(path, obj);
		}
	}
}

void XDataObject::slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type)
{
	
}