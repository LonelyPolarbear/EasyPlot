#include "XRefAttribute.h"
#include "XDataObject.h"

void XRefAttribute::setValue(sptr<XDataObject> data)
{
	if(data ==refData.lock())
		return;
	refData = data;
	emit_sigAttrChanged(XDataChangeType::ItemDataModified);
}

void XRefAttribute::setValueByPath(const XQ::XDataPath& path)
{
	auto parent =getParent();
	if(!parent)
		return;
	auto d =parent->getFromPath(path);
	setValue(d);
}

void XRefAttribute::serialize(HighFive::Group& group)
{
	if (auto p = getParent()) {
		if (auto data = value()) {
			auto path = p->getPathFromThis(data);
			group.createAttribute(getName(), path.path());
		}
	}
	group.createAttribute(getName(), "null");
}

void XRefAttribute::deserialize(HighFive::Group& group)
{
	auto h5Attr = group.getAttribute(getName());
	std::string str;
	h5Attr.read<std::string>(str);

	auto path =XQ::XDataPath::fromStr(str);
	if (!XDataObject::isDeserializing()) {
		XDataObject::setDeserializing(true);
		XDataObject::addDeserializeFinishedFn([path,this](){
			this->setValueByPath(path);
		});
	}
}

sptr<XDataObject> XRefAttribute::value()
{
	return refData.lock();
}
