#pragma once
#include "dataBaseApi.h"
#include "XDataBaseObject.h"
#include "xsignal/XSignal.h"
#include "XDataAttribute.h"
#include<vector>
#include "XDataPath.h"
namespace HighFive {
	class Group;
}
class DATABASE_API XDataObject : public XDataBaseObject {
	REGISTER_CLASS_META_DATA(XDataObject, XDataBaseObject);
protected:
	XDataObject(sptr<XDataObject> parent = nullptr);
	~XDataObject();
public:
	bool addData(sptr<XDataObject> data);
	bool removeData(sptr<XDataObject> data);
	sptr<XDataObject> getParent() const;
	void Init() override;
	void setBubble(bool flag);
	bool getBubble() const;
	void setName(const std::string& name);
	std::string getName() const;
	XQ::XDataPath getDataPath();
	XQ::XDataPath getPathFromThis(sptr<XDataObject> other, bool isAbs=false);
	
	sptr<XDataObject> getFromPath(const XQ::XDataPath& path);
	//序列化
	virtual void serialize(HighFive::Group& group);
	virtual void serializeData(HighFive::Group& group);
	//virtual void deserializeData(HighFive::Group& group);
	virtual void deserialize(HighFive::Group& group);
protected:
	sptr<XDataObject> getFromPathImp(XQ::XDataPath& path);
	friend class XDataList;
	bool addAttribute(sptr<XDataAttribute> attr);																																	//属性不允许删除
	bool hasData(sptr<XDataObject> data);
	bool hasAttribute(sptr<XDataAttribute> attr);
	void setParent(sptr<XDataObject> parent);
	
	void ItemDataChangedCB(sptr<XDataAttribute>, XDataChangeType type);
	void DataChangedCB(sptr<XDataObject>, XDataChangeType);																							//与sigDataChanged信号绑定，用于信号冒泡，向上传递
	void ChildItemDataChanged(XQ::XDataPath& path, sptr<XDataAttribute> attr, XDataChangeType type);
	void ChildDataChanged(XQ::XDataPath& path, sptr<XDataObject> obj, XDataChangeType type);
	void ChildDataAddChanged(XQ::XDataPath& path, sptr<XDataObject> obj);
	void ChildDataRemoveChanged(XQ::XDataPath& path, sptr<XDataObject> obj);
	void DataAddCB(sptr<XDataObject> data);
	void DataRemoveCB(sptr<XDataObject> data);
public:
	sptr<XDataAttribute> getAttribute(const std::string& name) const;
public:
	virtual void slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type);
	virtual void slotDataChanged(sptr<XDataObject>, XDataChangeType);		
	void beginBatch(bool propagateToChildren = true);
	void endBatch(bool propagateToChildren = true);
	bool isInBatch() const;
	virtual int childCount() const;
	virtual sptr<XDataObject> childAt(int index) const;

	virtual int attrCount() const;
	virtual sptr<XDataAttribute> attrAt(int index) const;

	int childIndex(sptr<XDataObject> child) const;
protected:
	void setBatchLevel(int level);
	//测试接口，用于信号发射时打印信息，因此封装为接口
	void notify_sigDataAdd(sptr<XDataObject> data);
	void notify_sigDataAdd(const XQ::XDataPath& path, sptr<XDataObject> data);
	void notify_sigDataRemove(sptr<XDataObject> data);
	void notify_sigDataRemove(const XQ::XDataPath& path,sptr<XDataObject> data);
public:
	XSIGNAL(void(sptr<XDataObject>, XDataChangeType))  sigDataChanged;																						//自身数据改变
	XSIGNAL(void(sptr<XDataAttribute>, XDataChangeType))  sigItemDataChanged;																			//自身属性改变
	XSIGNAL(void(sptr<XDataObject>))  sigDataAdd;																																//添加数据
	XSIGNAL(void(sptr<XDataObject>))  sigDataRemove;																															//删除数据
	XSIGNAL(void(const XQ::XDataPath&, sptr<XDataObject>,XDataChangeType))  sigChildDataChanged;											//子数据改变
	XSIGNAL(void(const XQ::XDataPath&, sptr<XDataAttribute>, XDataChangeType))  sigChildItemDataChanged;								//子属性改变
	XSIGNAL(void(const XQ::XDataPath&, sptr<XDataObject>))  sigChildDataAdd;																				//子数据添加
	XSIGNAL(void(const XQ::XDataPath&, sptr<XDataObject>))  sigChildDataRemove;																			//子数据删除
																													
public:
	friend class XDataAttribute;
	csptr<XAttr_Bool> AttrVisible;
	csptr<XAttr_String> AttrName;
protected:
	uint64_t mUid;																										//全局唯一标识
	std::vector<sptr<XDataObject>> mChilds;
	wptr<XDataObject > mParent;
	std::vector<sptr<XDataAttribute>> mAttributes;
	
	bool mEnableBubble = false;													//信号是否向上冒泡传递
	int m_batchLevel = 0;															// 批量嵌套深度
	bool m_hasPendingChanges = false;										// 是否有待处理的修改
public:
	static bool isDeserializing();
	static void setDeserializing(bool flag);
	static void addDeserializeFinishedFn(std::function<void()> f);
};

#define XQ_XDATA_ADD(_name_) \
do { \
		using _class_ = XTraits::traits_class<std::remove_const_t<decltype(_name_)> >::classType; \
		auto& __tmp__ = const_cast<std::shared_ptr<_class_>&>(_name_); \
		__tmp__ = makeShareDbObject<_class_>(); \
		addData(__tmp__); \
} while (false);