#pragma once
#include "databaseApi.h"
#include "XDataBaseObject.h"
#include "xsignal/XSignal.h"
class XDataAttribute;
class database_API XDataObject : public XDataBaseObject {
protected:
	XDataObject(sptr<XDataObject> parent = nullptr);
	~XDataObject();
public:
	bool addData(sptr<XDataObject> data);
	bool removeData(sptr<XDataObject> data);
	sptr<XDataObject> getParent() const;
protected:
	bool addAttribute(sptr<XDataAttribute> attr);	// Ù–‘≤ª‘ –Ì…æ≥˝
	bool hasData(sptr<XDataObject> data);
	bool hasAttribute(sptr<XDataAttribute> attr);
	void setParant(sptr<XDataObject> parent);
public:
	sptr<XDataAttribute> getAttribute(const std::string& name) const;
public:
	virtual void ItemDataChangedCB(sptr<XDataAttribute>, XDataChangeType type);
	virtual void slotItemDataChanged(sptr<XDataAttribute>, XDataChangeType type);

public:
	XSIGNAL(void(sptr<XDataObject>, XDataChangeType))  sigDataChanged;
	XSIGNAL(void(sptr<XDataAttribute>, XDataChangeType))  sigItemDataChanged;
protected:
	uint64_t mUid;
	std::vector<sptr<XDataObject>> mChilds;
	wptr<XDataObject > mParent;
	std::vector<sptr<XDataAttribute>> mAttributes;
};