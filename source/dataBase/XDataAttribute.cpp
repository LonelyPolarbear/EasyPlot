#include "XDataAttribute.h"
#include "XDataObject.h"

#include <atomic>
#include <highfive/H5File.hpp>
static std::atomic< uint64_t>  object_id_counter(0);


template class classProcessorFactory<1, void(HighFive::Group& group, sptr<XDataAttribute>)>;
template class classProcessorFactory<2, void(HighFive::Group& group, sptr<XDataAttribute>)>;

XDataAttribute::XDataAttribute():mUid(object_id_counter++),mName(std::to_string(mUid))
{
}

XDataAttribute::~XDataAttribute()
{
}

sptr<XDataObject> XDataAttribute::getParent() const
{
	return mParent.lock();
}

void XDataAttribute::callParentSlot(XDataChangeType type)
{
	if (auto p = getParent()) {
		p->ItemDataChangedCB(asDerived<XDataAttribute>(), type);
	}
}

void XDataAttribute::setVisible(bool v)
{
	if(isVisible() == v)
		return;
	mVisible = v;
	callParentSlot(XDataChangeType::ItemVisibleModified);
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

void XDataAttribute::setName(const std::string& name)
{
	//需要保证属性名的唯一性
	std::vector<std::string> existingNames;
	if(auto p = getParent()){
		for (auto attr : p->mAttributes) {
			if (attr.get() != this) {
				existingNames.push_back(attr->getName());
			}
		}
	}
	mName = makeUnique(existingNames,name);
}

void XDataAttribute::emit_sigAttrChanged(XDataChangeType type)
{
	auto parent = getParent();
	if (parent && parent->isInBatch()) {
		//...
	}
	else {
		sigAttrChanged(asDerived<XDataAttribute>(), type);
	}
}

void XDataAttribute::serialize(HighFive::Group& group)
{
	//group.createAttribute("className", getClassName());
	group.createAttribute(getName(),"null");
}

void XDataAttribute::deserialize(HighFive::Group& group)
{
}

template class DATABASE_API XDataAttributeT<bool>;
template class DATABASE_API XDataAttributeT<int>;
template class DATABASE_API XDataAttributeT<unsigned int>;
template class DATABASE_API XDataAttributeT<float>;
template class DATABASE_API XDataAttributeT<double>;
template class DATABASE_API XDataAttributeT<std::string>;

template class DATABASE_API XDataAttributeT<XQ::Vec2f>;
template class DATABASE_API XDataAttributeT<XQ::Vec2d>;
template class DATABASE_API XDataAttributeT<XQ::Vec2i>;
template class DATABASE_API XDataAttributeT<XQ::Vec2u>;
template class DATABASE_API XDataAttributeT<XQ::Vec2u8>;
template class DATABASE_API XDataAttributeT<XQ::Vec2i8>;

template class DATABASE_API XDataAttributeT<XQ::Vec3f>;
template class DATABASE_API XDataAttributeT<XQ::Vec3d>;
template class DATABASE_API XDataAttributeT<XQ::Vec3i>;
template class DATABASE_API XDataAttributeT<XQ::Vec3u>;
template class DATABASE_API XDataAttributeT<XQ::Vec3u8>;
template class DATABASE_API XDataAttributeT<XQ::Vec3i8>;

template class DATABASE_API XDataAttributeT<XQ::Vec4f>;
template class DATABASE_API XDataAttributeT<XQ::Vec4d>;
template class DATABASE_API XDataAttributeT<XQ::Vec4i>;
template class DATABASE_API XDataAttributeT<XQ::Vec4u>;
template class DATABASE_API XDataAttributeT<XQ::Vec4u8>;
template class DATABASE_API XDataAttributeT<XQ::Vec4i8>;

template class DATABASE_API XDataAttributeT<XQ::XColor>;
