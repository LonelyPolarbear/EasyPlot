#pragma once
#include "dataBaseApi.h"
#include "XDataBaseObject.h"
#include <xsignal/XSignal.h>
#include "XVector.h"
#include "XClolor.h"
#include "lib00_utilty/gp/XTraits.hpp"
#include <highfive/H5File.hpp>
#include <lib00_utilty/gp/classProcessorFactory.hpp>

#define ATTR_SERIALIZER 0
#define ATTR_DESERIALIZER 1

class XDataObject;
class DATABASE_API XDataAttribute : public XDataBaseObject {
	REGISTER_CLASS_META_DATA(XDataAttribute, XDataBaseObject);
protected:
	XDataAttribute();
	~XDataAttribute();
public:
	XSIGNAL(void(sptr<XDataAttribute>, XDataChangeType))  sigAttrChanged;
	friend class XDataObject;
	uint64_t getUid() const {
		return mUid;
	}
	sptr<XDataObject> getParent() const;

	void callParentSlot(XDataChangeType);

	void setVisible(bool v);

	void setName(const std::string& name);

	virtual void* getValuePtr()  {
		return nullptr;
	}

	const std::string& getName() const {
		return mName;
	}

	bool isVisible() const {
		return mVisible;
	}

	void emit_sigAttrChanged(XDataChangeType type);

	virtual void serialize(HighFive::Group& group);
	virtual void deserialize(HighFive::Group& group);
protected:
	wptr<XDataObject> mParent;
	uint64_t mUid;
	bool mVisible = true;
	std::string mName;
};


extern DATABASE_API classProcessorFactory<void(HighFive::Group& group, sptr<XDataAttribute>)> XattrSerializer;
extern DATABASE_API classProcessorFactory<void(HighFive::Group& group, sptr<XDataAttribute>)> XattrDeserializer;

extern  DATABASE_API classMultiProcessorFactory<
	void(sptr<XDataAttribute>,const std::string&),
	std::string(sptr<XDataAttribute>) 
> XattrToQstringFactory;


template<typename T>
class XDataAttributeT : public XDataAttribute {
	REGISTER_CLASS_META_DATA(XDataAttributeT<T>, XDataAttribute);
protected:
	XDataAttributeT()=default;
	~XDataAttributeT()=default;
public:
	void setValue(const T& v){
		if constexpr (XTraits::has_equal_operator_v<T>) {
			if (value == v) {
				return;
			}
		}
		
		value = v;
		Modified();
		emit_sigAttrChanged(XDataChangeType::ItemDataModified);
		callParentSlot(XDataChangeType::ItemDataModified);
	}
	const T& getValue() const {
		return value;
	}

	void* getValuePtr() override{
		return &value;
	}

	void serialize(HighFive::Group& group) override {
		auto name = XQ_META::ClassName<T>();
		if (XattrSerializer.hasProcessor(name)) {
			XattrSerializer.process(name , group,asDerived<XDataAttribute>());
		}
	}

	void deserialize(HighFive::Group& group) override {
		auto name = XQ_META::ClassName<T>();
		if (XattrDeserializer.hasProcessor(name)) {
			XattrDeserializer.process(name, group, asDerived<XDataAttribute>());
		}
	}
protected:
	T value{};
};

class XDataAttributeEnumBase : public XDataAttribute {
	REGISTER_CLASS_META_DATA(XDataAttributeEnumBase, XDataAttribute);
protected:
	XDataAttributeEnumBase() = default;
	~XDataAttributeEnumBase() = default;
public:
	void setIntValue(unsigned int v) {
		if (value == v) {
			return;
		}
		value = v;
		Modified();
		emit_sigAttrChanged(XDataChangeType::ItemDataModified);
		callParentSlot(XDataChangeType::ItemDataModified);
	}
	unsigned int getIntValue() const {
		return value;
	}

	void serialize(HighFive::Group& group) override {
		group.createAttribute(getName(), value);
	}
	void deserialize(HighFive::Group& group) override {
		//group.createAttribute(getName(), value);
		auto h5Attr = group.getAttribute(getName());
		unsigned int tmp{0};
		h5Attr.read<unsigned int>(tmp);
		setIntValue(tmp);
	}
protected:
	unsigned int value{};
};


template<typename T,typename = std::enable_if_t<std::is_enum_v<T>>>
class XDataAttributeEnum : public XDataAttributeEnumBase {
	REGISTER_CLASS_META_DATA(XDataAttributeEnum<T>, XDataAttributeEnumBase);
protected:
	XDataAttributeEnum() = default;
	~XDataAttributeEnum() = default;
public:
	void setValue(T v) {
		setIntValue((unsigned int)v);
	}
	T getValue() const {
		return (T)getIntValue();
	}
};

template<typename T>
using XAttr_Enum = XDataAttributeEnum<T>;

template<typename T>
std::string XattrEnumToString(sptr<XDataAttribute> attr_) {
	auto attr = attr_->asDerived<XDataAttributeEnum<T>>();
	const auto& value = attr->getValue();
	return XQ_META::enum_to_string(value);
}

extern template class DATABASE_API XDataAttributeT<int>;
extern template class DATABASE_API XDataAttributeT<bool>;
extern template class DATABASE_API XDataAttributeT<unsigned int>;
extern template class DATABASE_API XDataAttributeT<float>;
extern template class DATABASE_API XDataAttributeT<double>;
extern template class DATABASE_API XDataAttributeT<std::string>;

extern template class DATABASE_API XDataAttributeT<XQ::Vec2f>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec2d>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec2i>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec2u>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec2u8>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec2i8>;

extern template class DATABASE_API XDataAttributeT<XQ::Vec3f>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec3d>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec3i>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec3u>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec3u8>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec3i8>;

extern template class DATABASE_API XDataAttributeT<XQ::Vec4f>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec4d>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec4i>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec4u>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec4u8>;
extern template class DATABASE_API XDataAttributeT<XQ::Vec4i8>;

extern template class DATABASE_API XDataAttributeT<XQ::XColor>;

using XAttr_Color = XDataAttributeT<XQ::XColor>;
using XAttr_Int = XDataAttributeT<int>;
using XAttr_Bool = XDataAttributeT<bool>;
using XAttr_UInt = XDataAttributeT<unsigned int>;
using XAttr_Float = XDataAttributeT<float>;
using XAttr_Double = XDataAttributeT<double>;
using XAttr_String = XDataAttributeT<std::string>;
using XAttr_Vec2f = XDataAttributeT<XQ::Vec2f>;
using XAttr_Vec2d = XDataAttributeT<XQ::Vec2d>;
using XAttr_Vec2i = XDataAttributeT<XQ::Vec2i>;
using XAttr_Vec2u = XDataAttributeT<XQ::Vec2u>;
using XAttr_Vec2u8 = XDataAttributeT<XQ::Vec2u8>;
using XAttr_Vec2i8 = XDataAttributeT<XQ::Vec2i8>;
using XAttr_Vec3f = XDataAttributeT<XQ::Vec3f>;
using XAttr_Vec3d = XDataAttributeT<XQ::Vec3d>;
using XAttr_Vec3i = XDataAttributeT<XQ::Vec3i>;
using XAttr_Vec3u = XDataAttributeT<XQ::Vec3u>;
using XAttr_Vec3u8 = XDataAttributeT<XQ::Vec3u8>;
using XAttr_Vec3i8 = XDataAttributeT<XQ::Vec3i8>;
using XAttr_Vec4f = XDataAttributeT<XQ::Vec4f>;
using XAttr_Vec4d = XDataAttributeT<XQ::Vec4d>;
using XAttr_Vec4i = XDataAttributeT<XQ::Vec4i>;
using XAttr_Vec4u = XDataAttributeT<XQ::Vec4u>;
using XAttr_Vec4u8 = XDataAttributeT<XQ::Vec4u8>;
using XAttr_Vec4i8 = XDataAttributeT<XQ::Vec4i8>;
using XAttr_Point2f = XDataAttributeT<XQ::Point2f>; 
using XAttr_Point2d = XDataAttributeT<XQ::Point2d>; 
using XAttr_Point2i = XDataAttributeT<XQ::Point2i>; 
using XAttr_Point2u = XDataAttributeT<XQ::Point2u>;

using XAttr_Point3f = XDataAttributeT<XQ::Point3f>;
using XAttr_Point3d = XDataAttributeT<XQ::Point3d>;
using XAttr_Point3i = XDataAttributeT<XQ::Point3i>;
using XAttr_Point3u = XDataAttributeT<XQ::Point3u>;

using XAttr_Point4f = XDataAttributeT<XQ::Point4f>;
using XAttr_Point4d = XDataAttributeT<XQ::Point4d>;
using XAttr_Point4i = XDataAttributeT<XQ::Point4i>;
using XAttr_Point4u = XDataAttributeT<XQ::Point4u>;

using XAttr_Rectf = XDataAttributeT<XQ::Rectf>;
using XAttr_Rectd = XDataAttributeT<XQ::Rectd>;
using XAttr_Recti = XDataAttributeT<XQ::Recti>;
using XAttr_Rectu = XDataAttributeT<XQ::Rectu>;

#define XQ_ATTR_ADD(_name_) \
do { \
		using _class_ = XTraits::traits_class<std::remove_const_t<decltype(_name_)> >::classType; \
		auto& __tmp__ = const_cast<std::shared_ptr<_class_>&>(_name_); \
		__tmp__ = makeShareDbObject<_class_>(); \
		addAttribute(__tmp__); \
} while (false);

#define XQ_ATTR_ADD_INIT(_name_,_val_) \
do { \
		using _class_ = XTraits::traits_class<std::remove_const_t<decltype(_name_)> >::classType; \
		auto& __tmp__ = const_cast<std::shared_ptr<_class_>&>(_name_); \
		__tmp__ = makeShareDbObject<_class_>(); \
		__tmp__->setValue(_val_); \
		__tmp__->setName(#_name_); \
		addAttribute(__tmp__); \
} while (false);