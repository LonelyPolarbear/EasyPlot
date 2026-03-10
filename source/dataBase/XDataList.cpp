#include "XDataList.h"
#include "XObjectFactory.h"

XDataList::XDataList()
{
}

XDataList::~XDataList()
{
}

int XDataList::attrCount() const
{
	 return XDataObject::attrCount(); 
}

sptr<XDataAttribute> XDataList::attrAt(int index) const
{
	return XDataObject::attrAt(index);
}

void XDataList::serialize(HighFive::Group& group)
{
	XDataObject::serialize(group);
}

void XDataList::deserialize(HighFive::Group& group)
{
	//反序列化属性
	auto attrNum = group.getNumberAttributes();
	auto thisAttrNum = attrCount();
	for (int i = 0; i < thisAttrNum; i++) {
		auto a = attrAt(i);
		a->deserialize(group);
	}

	//反序列化数据

	std::string name;
	auto attr = group.getAttribute("className");
	attr.read(name);
	auto parents = XBaseObjectMeta::GetParents(name);
	if (parents.size() > 0) {
		//if (parents[0] == "XDataList") {
			//说明当前的节点是list,需要手动的动态添加
			//获取当前group下的子
			auto objNum = group.getNumberObjects();
			for (int i = 0; i < objNum; i++) {
				auto objName = group.getObjectName(i);
				if (objName == "__data__")
					continue;
				auto subGroup = group.getGroup(objName);
				std::string className;
				subGroup.getAttribute("className").read<std::string>(className);
				auto sub_object = XBaseObjectMeta::CreateObject(className);

				sub_object->asDerived<XDataObject>()->deserialize(subGroup);
				asDerived<XDataList>()->append(sub_object->asDerived<XDataObject>());
			}
		//}
	}
}
