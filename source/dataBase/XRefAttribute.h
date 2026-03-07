#include "dataBaseApi.h"
#include "XDataAttribute.h"
#include "XDataPath.h"
class XDataObject;
class DATABASE_API XRefAttribute :public XDataAttribute {
	REGISTER_CLASS_META_DATA(XRefAttribute, XDataAttribute);
public:
	void setValue(sptr<XDataObject> data);
	void setValueByPath(const XQ::XDataPath& path);

	virtual void serialize(HighFive::Group& group);
	virtual void deserialize(HighFive::Group& group);
	sptr<XDataObject> value();
protected:
	wptr< XDataObject> refData;
};