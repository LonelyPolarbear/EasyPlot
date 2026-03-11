#include "Init.h"
#include "XDataObjectTableModel.h"
#include <dataBase/XDataAttribute.h>
#include "XAttrItemDelegate.h"
template<typename T>
QString valueToString(const T& value) {
	if constexpr (std::is_arithmetic_v<T>) {
		return QString::number(value);
	}

	if constexpr (XTraits::is_xq_vector_v<T>) {
		return "xvector";
	}

	if constexpr (std::is_same_v<T, XQ::XColor>) {
		return "xcolor";
	}

	if constexpr (std::is_same_v<T, std::string>) {
		return QString::fromStdString(value);
	}

	return "unKnown";
}

template<typename T>
QString XattrToQstring(sptr<XDataAttribute> attr_) {
	auto attr =attr_->asDerived<XDataAttributeT<T>>();
	const auto & value =attr->getValue();
	return valueToString(value);
}

 void XMVD_Init(void)
{
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Color>(), XattrToQstring<XQ::XColor>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Int>(), XattrToQstring<int>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Bool>(), XattrToQstring<bool>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_UInt>(), XattrToQstring<unsigned int>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Float>(), XattrToQstring<float>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Double>(), XattrToQstring<double>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_String>(), XattrToQstring<std::string>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2f>(), XattrToQstring<XQ::Vec2f>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2d>(), XattrToQstring<XQ::Vec2d>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2i>(), XattrToQstring<XQ::Vec2i>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2u>(), XattrToQstring<XQ::Vec2u>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2u8>(), XattrToQstring<XQ::Vec2u8>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec2i8>(), XattrToQstring<XQ::Vec2i8>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3f>(), XattrToQstring<XQ::Vec3f>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3d>(), XattrToQstring<XQ::Vec3d>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3i>(), XattrToQstring<XQ::Vec3i>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3u>(), XattrToQstring<XQ::Vec3u>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3u8>(), XattrToQstring<XQ::Vec3u8>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec3i8>(), XattrToQstring<XQ::Vec3i8>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4f>(), XattrToQstring<XQ::Vec4f>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4d>(), XattrToQstring<XQ::Vec4d>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4i>(), XattrToQstring<XQ::Vec4i>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4u>(), XattrToQstring<XQ::Vec4u>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4u8>(), XattrToQstring<XQ::Vec4u8>);
	 XattrToQstringFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Vec4i8>(), XattrToQstring<XQ::Vec4i8>);

	 InitXAttrItemDelegate();
}
