#include "XObjectFactory.h"
#include "XDataAttribute.h"
#include "XDataBaseObject.h"
#include "XDataObject.h"
#include "XDataList.h"
#include "XClolor.h"
namespace XBaseObjectMeta {
	void InitAttrWrite();
	void InitAttrRead();
}
namespace XBaseObjectMeta {
	static std::map<std::string, std::vector<std::string>> xobject_inheritMap;
	template class  SFactory<std::string, XBaseObject(), void(), policyFactoryCreateDataobject>;

	extern DATABASE_API void RegisterInherit(const std::string& child, std::vector<std::string>& parents)
	{
		xobject_inheritMap[child] = parents;
	}

	std::optional<std::string> GetParent(const std::string child) {
		auto iter = xobject_inheritMap.find(child);
		if (iter == xobject_inheritMap.end()) {
			return std::nullopt;
		}
		else {
			if (iter->second.size() == 0) {
				return std::nullopt;
			}
			return iter->second[0];
		}
	}

	sptr<XBaseObject> DATABASE_API CreateObject(const std::string& className)
	{
		//确认是否注册
		auto product = XObjectFactory::Instance().makeShare(className);
		if (!product) {
			//尝试创建其父类
			auto parent = GetParent(className);
			if (parent) {
				return CreateObject(*parent);
			}
			else {
				return nullptr;
			}
		}
		return product;
	}

	DATABASE_API std::vector<std::string> GetParents(const std::string& child)
	{
		auto iter = xobject_inheritMap.find(child);
		if (iter == xobject_inheritMap.end()) {
			return {};
		}
		else {
			return iter->second;
		}
	}

	DATABASE_API void InitializeCoreTypes()
	{
		registerObject<XBaseObject>();
		registerObject<XDataBaseObject>();
		registerObject<XDataObject>();
		registerObject<XAttr_Color>();
		registerObject<XAttr_Int>();
		registerObject<XAttr_Bool>();
		registerObject<XAttr_UInt>();
		registerObject<XAttr_Float>();
		registerObject<XAttr_Double>();
		registerObject<XAttr_String>();
		registerObject<XAttr_Vec2f>();
		registerObject<XAttr_Vec2d>();
		registerObject<XAttr_Vec2i>();
		registerObject<XAttr_Vec2u>();
		registerObject<XAttr_Vec2u8>();
		registerObject<XAttr_Vec2i8>();
		registerObject<XAttr_Vec3f>();
		registerObject<XAttr_Vec3d>();
		registerObject<XAttr_Vec3i>();
		registerObject<XAttr_Vec3u>();
		registerObject<XAttr_Vec3u8>();
		registerObject<XAttr_Vec3i8>();
		registerObject<XAttr_Vec4f>();
		registerObject<XAttr_Vec4d>();
		registerObject<XAttr_Vec4i>();
		registerObject<XAttr_Vec4u>();
		registerObject<XAttr_Vec4u8>();
		registerObject<XAttr_Vec4i8>();
		registerObject<XAttr_Point2f>();
		registerObject<XAttr_Point2d>();
		registerObject<XAttr_Point2i>();
		registerObject<XAttr_Point2u>();
		registerObject<XAttr_Point3f>();
		registerObject<XAttr_Point3d>();
		registerObject<XAttr_Point3i>();
		registerObject<XAttr_Point3u>();
		registerObject<XAttr_Point4f>();
		registerObject<XAttr_Point4d>();
		registerObject<XAttr_Point4i>();
		registerObject<XAttr_Point4u>();
		registerObject<XAttr_Rectf>();
		registerObject<XAttr_Rectd>();
		registerObject<XAttr_Recti>();
		registerObject<XAttr_Rectu>();
		registerObject<XDataListT<XDataObject>>();

		InitAttrWrite();
		InitAttrRead();
	}
}

//属性写
namespace XBaseObjectMeta {
	template<typename T>
	struct writeAttr_vector;

	template<unsigned N, typename T>
	struct writeAttr_vector<XQ::Vector<N, T>> {
		static void write(HighFive::Group& group, sptr<XDataAttribute> attr) {
			auto ptr = (XQ::Vector<N, T>*)attr->getValuePtr();
			if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
				std::vector<int> vec;
				for (int i = 0; i < N; i++) {
					vec.push_back((*ptr)[i]);
				}
				group.createAttribute(attr->getName(), vec);
			}
			else {
				std::vector<T> vec;
				for (int i = 0; i < N; i++) {
					vec.push_back((*ptr)[i]);
				}
				group.createAttribute(attr->getName(), vec);
			}
		}
	};

	template<typename T>
	void writeAttr(HighFive::Group& group, sptr<XDataAttribute> attr) {
		if constexpr (XTraits::is_xq_vector_v<T>) {
			writeAttr_vector<T>::write(group, attr);
		}
		else {
			if constexpr (std::is_same_v<T, XQ::XColor>) {
				auto ptr = (XQ::XColor*)attr->getValuePtr();
				std::vector<int> data{ ptr->r(), ptr->g(), ptr->b(), ptr->a() };
				group.createAttribute(attr->getName(), data);
			}
			else {
				auto ptr = (T*)attr->getValuePtr();
				group.createAttribute(attr->getName(), *ptr);
			}
		}
	}

	void InitAttrWrite() {
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::XColor>(), writeAttr<XQ::XColor>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<int>(), writeAttr<int>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<bool>(), writeAttr<bool>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<unsigned int>(), writeAttr<unsigned int>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<float>(), writeAttr<float>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<double>(), writeAttr<double>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<std::string>(), writeAttr<std::string>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), writeAttr<XQ::Vec2f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), writeAttr<XQ::Vec2d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), writeAttr<XQ::Vec2i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), writeAttr<XQ::Vec2u>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), writeAttr<XQ::Vec2u8>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), writeAttr<XQ::Vec2i8>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), writeAttr<XQ::Vec3f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), writeAttr<XQ::Vec3d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), writeAttr<XQ::Vec3i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), writeAttr<XQ::Vec3u>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), writeAttr<XQ::Vec3u8>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), writeAttr<XQ::Vec3i8>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), writeAttr<XQ::Vec4f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), writeAttr<XQ::Vec4d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), writeAttr<XQ::Vec4i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), writeAttr<XQ::Vec4u>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), writeAttr<XQ::Vec4u8>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), writeAttr<XQ::Vec4i8>);

		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2f>(), writeAttr<XQ::Point2f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2d>(), writeAttr<XQ::Point2d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2i>(), writeAttr<XQ::Point2i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2u>(), writeAttr<XQ::Point2u>);

		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3f>(), writeAttr<XQ::Point3f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3d>(), writeAttr<XQ::Point3d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3i>(), writeAttr<XQ::Point3i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3u>(), writeAttr<XQ::Point3u>);

		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4f>(), writeAttr<XQ::Point4f>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4d>(), writeAttr<XQ::Point4d>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4i>(), writeAttr<XQ::Point4i>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4u>(), writeAttr<XQ::Point4u>);

		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectf>(), writeAttr<XQ::Rectf>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectd>(), writeAttr<XQ::Rectd>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Recti>(), writeAttr<XQ::Recti>);
		XattrSerializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectu>(), writeAttr<XQ::Rectu>);
	}
}

//属性读
namespace XBaseObjectMeta {
	template<typename T>
	struct readAttr_vector;

	template<unsigned N, typename T>
	struct readAttr_vector<XQ::Vector<N, T>> {
		static void read(HighFive::Group& group, sptr<XDataAttribute> attr) {
			//获取属性
			if (!group.hasAttribute(attr->getName())) {
				return;
			}
			auto h5Attr = group.getAttribute(attr->getName());
			auto ptr = (XQ::Vector<N, T>*)attr->getValuePtr();
			if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
				std::vector<int> vec;
				h5Attr.read<std::vector<int>>(vec);
				XQ::Vector<N, T> real_data;
				for (int i = 0; i < N; i++) {
					real_data[i] = vec[i];
				}
				
				auto real_attr = attr->asDerived<XDataAttributeT<XQ::Vector<N, T>>>();
				real_attr->setValue(real_data);
			}
			else {
				std::vector<T> vec;
				h5Attr.read<std::vector<T>>(vec);
				XQ::Vector<N, T> real_data;
				for (int i = 0; i < N; i++) {
					real_data[i] = vec[i];
				}

				auto real_attr =attr->asDerived<XDataAttributeT<XQ::Vector<N, T>>>();
				real_attr->setValue(real_data);
			}
		}
	};

	template<typename T>
	void readAttr(HighFive::Group& group, sptr<XDataAttribute> attr) {
		if (!group.hasAttribute(attr->getName())) {
			return;
		}
		auto h5Attr = group.getAttribute(attr->getName());
		if constexpr (XTraits::is_xq_vector_v<T>) {
			readAttr_vector<T>::read(group, attr);
		}
		else {
			if constexpr (std::is_same_v<T, XQ::XColor>) {
				std::vector<int> h5_value;
				h5Attr.read<std::vector<int>>(h5_value);
				XQ::XColor color(h5_value[0], h5_value[1], h5_value[2], h5_value[3]);
				auto real_attr = attr->asDerived<XDataAttributeT<XQ::XColor>>();
				real_attr->setValue(color);
			}
			else {
				T h5_value{};
				h5Attr.read<T>(h5_value);
				auto real_attr = attr->asDerived<XDataAttributeT<T>>();
				real_attr->setValue(h5_value);
			}
		}
	}

	void InitAttrRead(){

		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::XColor>(), readAttr<XQ::XColor>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<int>(), readAttr<int>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<bool>(), readAttr<bool>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<unsigned int>(), readAttr<unsigned int>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<float>(), readAttr<float>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<double>(), readAttr<double>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<std::string>(), readAttr<std::string>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), readAttr<XQ::Vec2f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), readAttr<XQ::Vec2d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), readAttr<XQ::Vec2i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), readAttr<XQ::Vec2u>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), readAttr<XQ::Vec2u8>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), readAttr<XQ::Vec2i8>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), readAttr<XQ::Vec3f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), readAttr<XQ::Vec3d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), readAttr<XQ::Vec3i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), readAttr<XQ::Vec3u>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), readAttr<XQ::Vec3u8>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), readAttr<XQ::Vec3i8>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), readAttr<XQ::Vec4f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), readAttr<XQ::Vec4d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), readAttr<XQ::Vec4i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), readAttr<XQ::Vec4u>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), readAttr<XQ::Vec4u8>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), readAttr<XQ::Vec4i8>);

		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2f>(), readAttr<XQ::Point2f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2d>(), readAttr<XQ::Point2d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2i>(), readAttr<XQ::Point2i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point2u>(), readAttr<XQ::Point2u>);

		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3f>(), readAttr<XQ::Point3f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3d>(), readAttr<XQ::Point3d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3i>(), readAttr<XQ::Point3i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point3u>(), readAttr<XQ::Point3u>);

		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4f>(), readAttr<XQ::Point4f>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4d>(), readAttr<XQ::Point4d>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4i>(), readAttr<XQ::Point4i>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Point4u>(), readAttr<XQ::Point4u>);

		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectf>(), readAttr<XQ::Rectf>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectd>(), readAttr<XQ::Rectd>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Recti>(), readAttr<XQ::Recti>);
		XattrDeserializer::instance().registerProcessor(XQ_META::ClassName<XQ::Rectu>(), readAttr<XQ::Rectu>);
	}
}