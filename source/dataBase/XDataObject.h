#pragma once
#include "databaseApi.h"
#include "XDataBaseObject.h"
#include "xsignal/XSignal.h"
#include "XDataAttribute.h"
#include<vector>
//class XDataAttribute;
//class XAttr_Bool;
//class XAttr_String;

class database_API XDataObject : public XDataBaseObject {
protected:
	XDataObject(sptr<XDataObject> parent = nullptr);
	~XDataObject();
public:
	bool addData(sptr<XDataObject> data);
	bool removeData(sptr<XDataObject> data);
	sptr<XDataObject> getParent() const;
	void Init() override;
protected:
	bool addAttribute(sptr<XDataAttribute> attr);																																	//属性不允许删除
	bool hasData(sptr<XDataObject> data);
	bool hasAttribute(sptr<XDataAttribute> attr);
	void setParant(sptr<XDataObject> parent);
	
	virtual void ItemDataChangedCB(sptr<XDataAttribute>, XDataChangeType type);
	virtual void DataChangedCB(sptr<XDataObject>, XDataChangeType);																							//与sigDataChanged信号绑定，用于信号冒泡，向上传递
	virtual void ChildItemDataChanged(std::vector<std::string>& path, sptr<XDataAttribute> attr, XDataChangeType type);
	virtual void ChildDataChanged(std::vector<std::string>& path, sptr<XDataObject> obj, XDataChangeType type);
public:
	sptr<XDataAttribute> getAttribute(const std::string& name) const;
public:
	virtual void slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type);
	virtual void slotDataChanged(sptr<XDataObject>, XDataChangeType);		
	
public:
	XSIGNAL(void(sptr<XDataObject>, XDataChangeType))  sigDataChanged;																						//自身数据改变
	XSIGNAL(void(sptr<XDataAttribute>, XDataChangeType))  sigItemDataChanged;																			//自身属性改变
	XSIGNAL(void(const std::vector<std::string>&, sptr<XDataObject>,XDataChangeType))  sigChildDataChanged;																		//子数据改变
	XSIGNAL(void(const std::vector<std::string>&, sptr<XDataAttribute>, XDataChangeType))  sigChildItemDataChanged;							//子属性改变
	XSIGNAL(void(sptr<XDataObject>))  sigDataAdd;																																//添加数据
	XSIGNAL(void(sptr<XDataObject>))  sigDataRemove;																															//删除数据
public:
	friend class XDataAttribute;
	csptr<XAttr_Bool> AttrVisible;
	csptr<XAttr_String> AttrName;
protected:
	uint64_t mUid;																										//全局唯一标识
	std::vector<sptr<XDataObject>> mChilds;
	wptr<XDataObject > mParent;
	std::vector<sptr<XDataAttribute>> mAttributes;
	
	bool mEnableBubble = false;		//信号是否向上冒泡传递
};