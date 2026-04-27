#include "XObjectFactory.h"
#include "XDataAttribute.h"
#include "XDataBaseObject.h"
#include "XDataObject.h"
#include "XDataList.h"
#include "XClolor.h"
#include "XRefAttribute.h"
#include "XDataArray.h"
#include <lib00_utilty/XUtilty.h>
#include <regex>
namespace XBaseObjectMeta {
	void InitAttrWrite();
	void InitAttrRead();
	void InitDataWrite();
	void InitDataRead();
	void InitAttrToString();
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

	DATABASE_API bool IsA(const std::string& className, const std::string& baseClassName)
	{	
		if(className == baseClassName)
			return true;
		auto parents = GetParents(className);
		if (parents.size() > 0) {
			if (std::find(parents.begin(), parents.end(), className) != parents.end()) {
				return true;
			}
			else {
				//遍历每一父类
				for (auto parent : parents) {
					if (IsA(parent, baseClassName)) {
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}


	DATABASE_API void InitializeCoreTypes()
	{
		registerObject<XIntArray>();
		registerObject<XUIntArray>();
		registerObject<XFloatArray>();
		registerObject<XDoubleArray>();
		registerObject<XUCharArray>();
		registerObject<XCharArray>();

		registerObject<XIntArray2D>();
		registerObject<XUIntArray2D>();
		registerObject<XFloatArray2D>();
		registerObject<XDoubleArray2D>();
		registerObject<XUCharArray2D>();
		registerObject<XCharArray2D>();

		registerObject<XIntArray3D>();
		registerObject<XUIntArray3D>();
		registerObject<XFloatArray3D>();
		registerObject<XDoubleArray3D>();
		registerObject<XUCharArray3D>();
		registerObject<XCharArray3D>();

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
		registerObject<XRefAttribute>();

		InitAttrWrite();
		InitAttrRead();

		InitDataWrite();
		InitDataRead();

		InitAttrToString();
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
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::XColor>(), writeAttr<XQ::XColor>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<int>(), writeAttr<int>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<bool>(), writeAttr<bool>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<unsigned int>(), writeAttr<unsigned int>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<float>(), writeAttr<float>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<double>(), writeAttr<double>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<std::string>(), writeAttr<std::string>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), writeAttr<XQ::Vec2f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), writeAttr<XQ::Vec2d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), writeAttr<XQ::Vec2i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), writeAttr<XQ::Vec2u>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), writeAttr<XQ::Vec2u8>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), writeAttr<XQ::Vec2i8>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), writeAttr<XQ::Vec3f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), writeAttr<XQ::Vec3d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), writeAttr<XQ::Vec3i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), writeAttr<XQ::Vec3u>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), writeAttr<XQ::Vec3u8>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), writeAttr<XQ::Vec3i8>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), writeAttr<XQ::Vec4f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), writeAttr<XQ::Vec4d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), writeAttr<XQ::Vec4i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), writeAttr<XQ::Vec4u>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), writeAttr<XQ::Vec4u8>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), writeAttr<XQ::Vec4i8>);

		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point2f>(), writeAttr<XQ::Point2f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point2d>(), writeAttr<XQ::Point2d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point2i>(), writeAttr<XQ::Point2i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point2u>(), writeAttr<XQ::Point2u>);

		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point3f>(), writeAttr<XQ::Point3f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point3d>(), writeAttr<XQ::Point3d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point3i>(), writeAttr<XQ::Point3i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point3u>(), writeAttr<XQ::Point3u>);

		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point4f>(), writeAttr<XQ::Point4f>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point4d>(), writeAttr<XQ::Point4d>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point4i>(), writeAttr<XQ::Point4i>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Point4u>(), writeAttr<XQ::Point4u>);

		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Rectf>(), writeAttr<XQ::Rectf>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Rectd>(), writeAttr<XQ::Rectd>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Recti>(), writeAttr<XQ::Recti>);
		XattrSerializer.registerProcessor(XQ_META::ClassName<XQ::Rectu>(), writeAttr<XQ::Rectu>);
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

		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::XColor>(), readAttr<XQ::XColor>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<int>(), readAttr<int>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<bool>(), readAttr<bool>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<unsigned int>(), readAttr<unsigned int>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<float>(), readAttr<float>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<double>(), readAttr<double>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<std::string>(), readAttr<std::string>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), readAttr<XQ::Vec2f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), readAttr<XQ::Vec2d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), readAttr<XQ::Vec2i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), readAttr<XQ::Vec2u>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), readAttr<XQ::Vec2u8>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), readAttr<XQ::Vec2i8>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), readAttr<XQ::Vec3f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), readAttr<XQ::Vec3d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), readAttr<XQ::Vec3i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), readAttr<XQ::Vec3u>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), readAttr<XQ::Vec3u8>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), readAttr<XQ::Vec3i8>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), readAttr<XQ::Vec4f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), readAttr<XQ::Vec4d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), readAttr<XQ::Vec4i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), readAttr<XQ::Vec4u>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), readAttr<XQ::Vec4u8>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), readAttr<XQ::Vec4i8>);

		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point2f>(), readAttr<XQ::Point2f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point2d>(), readAttr<XQ::Point2d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point2i>(), readAttr<XQ::Point2i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point2u>(), readAttr<XQ::Point2u>);

		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point3f>(), readAttr<XQ::Point3f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point3d>(), readAttr<XQ::Point3d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point3i>(), readAttr<XQ::Point3i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point3u>(), readAttr<XQ::Point3u>);

		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point4f>(), readAttr<XQ::Point4f>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point4d>(), readAttr<XQ::Point4d>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point4i>(), readAttr<XQ::Point4i>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Point4u>(), readAttr<XQ::Point4u>);

		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Rectf>(), readAttr<XQ::Rectf>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Rectd>(), readAttr<XQ::Rectd>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Recti>(), readAttr<XQ::Recti>);
		XattrDeserializer.registerProcessor(XQ_META::ClassName<XQ::Rectu>(), readAttr<XQ::Rectu>);
	}
}

//数据写
namespace XBaseObjectMeta {
	template<typename T>
	struct writeVecData;

	template<unsigned N, typename T>
	struct writeVecData<XQ::Vector<N, T>> {
		static void write(HighFive::Group& group, const std::string&name, void*data) {
			XQ::Vector<N, T>* pdata = (XQ::Vector<N, T>*)(data);

			auto  dataset = group.createDataSet(name, HighFive::DataSpace{N,1 }, HighFive::create_datatype<T>());
			dataset.write_raw(pdata->data, HighFive::create_datatype<T>());
			dataset.createAttribute("className", XQ_META::ClassName<XQ::Vector<N, T>>());
		}
	};

	template<typename T>
	void writeData(HighFive::Group& group, const std::string& name, void* data) {
		if constexpr (XTraits::is_xq_vector_v<T>) {
			writeVecData<T>::write(group, name,data);
		}
		else {
			if constexpr (std::is_same_v<T, XQ::XColor>) {
				XQ::XColor* pdata = (XQ::XColor*)(data);

				XQ::Vec4u8 color(pdata->r(), pdata->g(), pdata->b(), pdata->a() );

				auto  dataset = group.createDataSet(name, HighFive::DataSpace{ 4,1 }, HighFive::create_datatype<unsigned char>());
				dataset.write_raw(color.data, HighFive::create_datatype<unsigned char>());
				dataset.createAttribute("className", XQ_META::ClassName<T>());
			}
			else {
				T* pdata = (T*)(data);
				auto  dataset = group.createDataSet(name, *pdata);
				dataset.createAttribute("className", XQ_META::ClassName<T>());
			}
		}
	}

	template<typename T>
	void writeDataArrayID(HighFive::Group& group, const std::string& name, void* data) {
		using value_type = typename T::value_type;
		T* pdata = (T*)(data);
		auto size =pdata->size();
		auto vecPtr =pdata->getPtr();
		auto  dataset = group.createDataSet(name, HighFive::DataSpace{size,1}, HighFive::create_datatype<value_type>());
		dataset.write(*vecPtr);
		dataset.createAttribute("className", XQ_META::ClassName<T>());
		dataset.createAttribute("component", pdata->getComponent());
		dataset.createAttribute("name", pdata->getName());
	}

	template<typename T>
	void writeDataArray2D(HighFive::Group& group, const std::string& name, void* data) {
		using value_type = typename T::value_type;
		T* pdata = (T*)(data);
		auto size = pdata->size();
		auto vecPtr = pdata->getPtr();
		int component = pdata->getComponent();
		auto row = pdata->getRow();
		auto col = pdata->getCol();

		//按行存储，每一行存储的数据总长度component*col
		auto  dataset = group.createDataSet(name, HighFive::DataSpace{ row,component * col }, HighFive::create_datatype<value_type>());
		dataset.write_raw(vecPtr->data(), HighFive::create_datatype<value_type>());
		dataset.createAttribute("className", XQ_META::ClassName<T>());
		dataset.createAttribute("component", component);
		dataset.createAttribute("row", row);
		dataset.createAttribute("col", col);
		dataset.createAttribute("name", pdata->getName());
	}

	template<typename T>
	void writeDataArray3D(HighFive::Group& group, const std::string& name, void* data) {
		using value_type = typename T::value_type;
		T* pdata = (T*)(data);
		auto size = pdata->size();
		auto vecPtr = pdata->getPtr();
		int component = pdata->getComponent();
		auto row = pdata->getRow();
		auto col = pdata->getCol();
		auto depth = pdata->getDepth();

		//按行存储，每一行存储的数据总长度component*col
		auto  dataset = group.createDataSet(name, HighFive::DataSpace{ depth,row,component * col }, HighFive::create_datatype<value_type>());
		dataset.write_raw(vecPtr->data(), HighFive::create_datatype<value_type>());
		dataset.createAttribute("className", XQ_META::ClassName<T>());
		dataset.createAttribute("component", component);
		dataset.createAttribute("row", row);
		dataset.createAttribute("col", col);
		dataset.createAttribute("depth", depth);
		dataset.createAttribute("name", pdata->getName());
	}
	
	void InitDataWrite() {
		XDataSerializer.registerProcessor(XQ_META::ClassName<int>(), writeData<int>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<double>(), writeData<double>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<float>(), writeData<float>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<unsigned int>(), writeData<unsigned int>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::XColor>(), writeData<XQ::XColor>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XIntArray>(), writeDataArrayID<XIntArray>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUIntArray>(), writeDataArrayID<XUIntArray>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XFloatArray>(), writeDataArrayID<XFloatArray>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XDoubleArray>(), writeDataArrayID<XDoubleArray>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUCharArray>(), writeDataArrayID<XUCharArray>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XCharArray>(), writeDataArrayID<XCharArray>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XIntArray2D>(), writeDataArray2D<XIntArray2D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUIntArray2D>(), writeDataArray2D<XUIntArray2D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XFloatArray2D>(), writeDataArray2D<XFloatArray2D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XDoubleArray2D>(), writeDataArray2D<XDoubleArray2D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUCharArray2D>(), writeDataArray2D<XUCharArray2D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XCharArray2D>(), writeDataArray2D<XCharArray2D>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XIntArray3D>(), writeDataArray3D<XIntArray3D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUIntArray3D>(), writeDataArray3D<XUIntArray3D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XFloatArray3D>(), writeDataArray3D<XFloatArray3D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XDoubleArray3D>(), writeDataArray3D<XDoubleArray3D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XUCharArray3D>(), writeDataArray3D<XUCharArray3D>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XCharArray3D>(), writeDataArray3D<XCharArray3D>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), writeData<XQ::Vec2f>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), writeData<XQ::Vec2d>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), writeData<XQ::Vec2i>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), writeData<XQ::Vec2u>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), writeData<XQ::Vec2u8>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), writeData<XQ::Vec2i8>);

		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), writeData<XQ::Vec3f>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), writeData<XQ::Vec3d>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), writeData<XQ::Vec3i>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), writeData<XQ::Vec3u>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), writeData<XQ::Vec3u8>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), writeData<XQ::Vec3i8>);


		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), writeData<XQ::Vec4f>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), writeData<XQ::Vec4d>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), writeData<XQ::Vec4i>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), writeData<XQ::Vec4u>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), writeData<XQ::Vec4u8>);
		XDataSerializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), writeData<XQ::Vec4i8>);

	}
}

//数据读
namespace XBaseObjectMeta {

	template<typename T>
	struct readVecData;

	template<unsigned N, typename T>
	struct readVecData<XQ::Vector<N, T>> {
		static void read(HighFive::DataSet& dataset, void* data) {
			XQ::Vector<N, T>* pdata = (XQ::Vector<N, T>*)(data);
			std::vector<T> vec;
			dataset.read(vec);
			for (int i = 0; i < N; i++) {
				pdata->data[i] = vec[i];
			}
		}
	};

	template<typename T>
	void readData(HighFive::DataSet& dataset, void* data) {
		if constexpr (XTraits::is_xq_vector_v<T>) {
			readVecData<T>::read(dataset, data);
		}
		else {
			if constexpr (std::is_same_v<T, XQ::XColor>) {
				std::vector<unsigned char> vec;
				dataset.read(vec);
				XQ::XColor* pdata = (XQ::XColor*)(data);
				pdata->setData(vec[0], vec[1], vec[2], vec[3]);
			}
			else {
				T* pdata = (T*)(data);
				dataset.read(*pdata);
			}
		}
	}

	template<typename T>
	void readDataArrayID(HighFive::DataSet& dataset, void* data) {
		using value_type = typename T::value_type;
		std::vector<value_type> vec;
		dataset.read(vec);
		T* pdata = (T*)(data);
		auto vecPtr = pdata->getPtr();
		vecPtr->swap(vec);

		int component =1;
		std::string name;
		dataset.getAttribute("component").read(component);
		dataset.getAttribute("name").read(name);

		pdata->setComponent(component);
		pdata->setName(name);
	}

	template<typename T>
	void readDataArray2D(HighFive::DataSet& dataset, void* data) {
		using value_type = typename T::value_type;

		int component = 1;
		unsigned int row = 1;
		unsigned int col = 1;
		std::string name;
		dataset.getAttribute("component").read(component);
		dataset.getAttribute("row").read(row);
		dataset.getAttribute("col").read(col);
		dataset.getAttribute("name").read(name);

		T* pdata = (T*)(data);
		pdata->setComponent(component);
		pdata->setName(name);
		pdata->setDimensions(col, row);
		auto vecPtr = pdata->getPtr();

		dataset.read(vecPtr->data());
	}

	template<typename T>
	void readDataArray3D(HighFive::DataSet& dataset, void* data) {
		using value_type = typename T::value_type;

		int component = 1;
		unsigned int row = 1;
		unsigned int col = 1;
		unsigned int depth = 1;
		std::string name;
		dataset.getAttribute("component").read(component);
		dataset.getAttribute("row").read(row);
		dataset.getAttribute("col").read(col);
		dataset.getAttribute("depth").read(depth);
		dataset.getAttribute("name").read(name);

		T* pdata = (T*)(data);
		pdata->setComponent(component);
		pdata->setName(name);
		pdata->setDimensions(col, row,depth);
		auto vecPtr = pdata->getPtr();

		dataset.read(vecPtr->data());
	}

	void InitDataRead() {
		XDataDeserializer.registerProcessor(XQ_META::ClassName<int>(), readData<int>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<double>(), readData<double>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<float>(), readData<float>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<unsigned int>(), readData<unsigned int>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::XColor>(), readData<XQ::XColor>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XIntArray>(), readDataArrayID<XIntArray>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUIntArray>(), readDataArrayID<XUIntArray>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XFloatArray>(), readDataArrayID<XFloatArray>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XDoubleArray>(), readDataArrayID<XDoubleArray>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUCharArray>(), readDataArrayID<XUCharArray>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XCharArray>(), readDataArrayID<XCharArray>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XIntArray2D>(), readDataArray2D<XIntArray2D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUIntArray2D>(), readDataArray2D<XUIntArray2D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XFloatArray2D>(), readDataArray2D<XFloatArray2D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XDoubleArray2D>(), readDataArray2D<XDoubleArray2D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUCharArray2D>(), readDataArray2D<XUCharArray2D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XCharArray2D>(), readDataArray2D<XCharArray2D>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XIntArray3D>(), readDataArray3D<XIntArray3D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUIntArray3D>(), readDataArray3D<XUIntArray3D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XFloatArray3D>(), readDataArray3D<XFloatArray3D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XDoubleArray3D>(), readDataArray3D<XDoubleArray3D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XUCharArray3D>(), readDataArray3D<XUCharArray3D>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XCharArray3D>(), readDataArray3D<XCharArray3D>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2f>(), readData<XQ::Vec2f>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2d>(), readData<XQ::Vec2d>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i>(), readData<XQ::Vec2i>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u>(), readData<XQ::Vec2u>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2u8>(), readData<XQ::Vec2u8>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec2i8>(), readData<XQ::Vec2i8>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3f>(), readData<XQ::Vec3f>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3d>(), readData<XQ::Vec3d>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i>(), readData<XQ::Vec3i>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u>(), readData<XQ::Vec3u>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3u8>(), readData<XQ::Vec3u8>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec3i8>(), readData<XQ::Vec3i8>);

		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4f>(), readData<XQ::Vec4f>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4d>(), readData<XQ::Vec4d>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i>(), readData<XQ::Vec4i>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u>(), readData<XQ::Vec4u>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4u8>(), readData<XQ::Vec4u8>);
		XDataDeserializer.registerProcessor(XQ_META::ClassName<XQ::Vec4i8>(), readData<XQ::Vec4i8>);
	}
}

namespace XBaseObjectMeta {
	//to string

	template<typename T>
	std::string valueToString(const T& value) {
		if constexpr (std::is_arithmetic_v<T>) {
			return std::to_string(value);
		}

		if constexpr (XTraits::is_xq_vector_v<T>) {
			std::stringstream stm;
			stm << value;
			return stm.str();
		}

		if constexpr (std::is_same_v<T, XQ::XColor>) {
			return "xcolor";
		}

		if constexpr (std::is_same_v<T, std::string>) {
			return value;
		}

		return "unKnown";
	}

	template<typename T>
	std::string XattrToString(sptr<XDataAttribute> attr_) {
		auto attr = attr_->asDerived<XDataAttributeT<T>>();
		const auto& value = attr->getValue();
		return valueToString(value);
	}


	std::string  XattrEnumBaseToQstring(sptr<XDataAttribute> attr_) {
		auto attr = attr_->asDerived<XDataAttributeEnumBase>();
		const auto& value = attr->getIntValue();
		return valueToString(value);
	}

	//from string
	template< typename T>
	struct VecDataFromStr;

	template<unsigned N, typename T>
	struct VecDataFromStr<XQ::Vector<N, T>> {
		static XQ::Vector<N, T> resume(const std::string& name) {
			XQ::Vector<N, T> data;
			//利用正则表达式，按照“,”分割字符串”
			//std::regex digit_regex(R"(\d+)");
			std::regex comma(",");

			std::sregex_token_iterator iter(name.begin(), name.end(), comma, -1);
			std::sregex_token_iterator end;

			std::vector<std::string> numbers(iter, end);

			// 遍历所有匹配项
			/*for (; iter != end; ++iter) {
				numbers.push_back(iter->str());
			}*/

			if (numbers.size() != N) {

				throw std::runtime_error("invalid vector data");
			}

			for (int i = 0; i < N; i++) {
				data.data[i] = XQ::fromString<T>(numbers[i]);
			}

			return data;
		}
	};

	template<typename T>
	void XattrFromString(sptr<XDataAttribute> attr_, const std::string& str) {
		auto attr = attr_->asDerived<XDataAttributeT<T>>();
		if constexpr (std::is_arithmetic_v<T>) {
			auto value = XQ::fromString<T>(str);
			attr->setValue(value);
		}

		if constexpr (XTraits::is_xq_vector_v<T>) {
			auto data = VecDataFromStr<T>::resume(str);
			attr->setValue(data);
			return;
		}

		if constexpr (std::is_same_v<T, XQ::XColor>) {
			return ;
		}

		if constexpr (std::is_same_v<T, std::string>) {
			attr->setValue(str);
			return ;
		}
	}

	void  XattrEnumBaseFromQstring(sptr<XDataAttribute> attr_,const std::string& str) {
		auto attr = attr_->asDerived<XDataAttributeEnumBase>();
		attr->setIntValue(std::stoi(str));
	}

	void InitAttrToString() {
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Color>(), XattrToString<XQ::XColor>); 
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Int>(), XattrToString<int>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Bool>(), XattrToString<bool>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_UInt>(), XattrToString<unsigned int>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Float>(), XattrToString<float>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Double>(), XattrToString<double>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_String>(), XattrToString<std::string>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2f>(), XattrToString<XQ::Vec2f>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2d>(), XattrToString<XQ::Vec2d>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2i>(), XattrToString<XQ::Vec2i>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2u>(), XattrToString<XQ::Vec2u>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2u8>(), XattrToString<XQ::Vec2u8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec2i8>(), XattrToString<XQ::Vec2i8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3f>(), XattrToString<XQ::Vec3f>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3d>(), XattrToString<XQ::Vec3d>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3i>(), XattrToString<XQ::Vec3i>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3u>(), XattrToString<XQ::Vec3u>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3u8>(), XattrToString<XQ::Vec3u8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec3i8>(), XattrToString<XQ::Vec3i8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4f>(), XattrToString<XQ::Vec4f>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4d>(), XattrToString<XQ::Vec4d>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4i>(), XattrToString<XQ::Vec4i>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4u>(), XattrToString<XQ::Vec4u>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4u8>(), XattrToString<XQ::Vec4u8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Vec4i8>(), XattrToString<XQ::Vec4i8>);
		XattrToQstringFactory.registerProcessor<1>(XQ_META::ClassName<XDataAttributeEnumBase>(), XattrEnumBaseToQstring);

		
		//XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Color>(), XattrFromString<>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Int>(), XattrFromString<int>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Bool>(), XattrFromString<bool>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_UInt>(), XattrFromString<unsigned int>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Float>(), XattrFromString<float>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Double>(), XattrFromString<double>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_String>(), XattrFromString<std::string>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2f>(), XattrFromString<XQ::Vec2f>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2d>(), XattrFromString<XQ::Vec2d>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2i>(), XattrFromString<XQ::Vec2i>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2u>(), XattrFromString<XQ::Vec2u>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2u8>(), XattrFromString<XQ::Vec2u8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec2i8>(), XattrFromString<XQ::Vec2i8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3f>(), XattrFromString<XQ::Vec3f>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3d>(), XattrFromString<XQ::Vec3d>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3i>(), XattrFromString<XQ::Vec3i>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3u>(), XattrFromString<XQ::Vec3u>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3u8>(), XattrFromString<XQ::Vec3u8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec3i8>(), XattrFromString<XQ::Vec3i8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4f>(), XattrFromString<XQ::Vec4f>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4d>(), XattrFromString<XQ::Vec4d>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4i>(), XattrFromString<XQ::Vec4i>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4u>(), XattrFromString<XQ::Vec4u>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4u8>(), XattrFromString<XQ::Vec4u8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Vec4i8>(), XattrFromString<XQ::Vec4i8>);
		XattrToQstringFactory.registerProcessor<0>(XQ_META::ClassName<XDataAttributeEnumBase>(), XattrEnumBaseFromQstring);
	}																		 
}