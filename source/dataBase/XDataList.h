#include "XDataObject.h"
#include "dataBaseApi.h"
#include <type_traits>

class DATABASE_API XDataList:public XDataObject {
	REGISTER_CLASS_META_DATA(XDataList, XDataObject);

	using iterator = typename std::vector<sptr<XDataObject>>::iterator;
	using const_iterator = typename std::vector<sptr<XDataObject>>::const_iterator;
protected:
	XDataList();
	~XDataList();
public:
	bool has(sptr<XDataObject> data) {
		for (auto t : mDataList) {
			if (t == data)
				return true;
		}
		return false;
	}

	virtual bool append(sptr<XDataObject> data){
		if (has(data)) {
			return false;
		}
		if (!data->getParent()) {
			data->setParent(asDerived<XDataObject>());
		}
		else {
			if (data->getParent() != asDerived<XDataObject>())
				return false;
		}

		mDataList.push_back(data);
		//需要保证data的名称的唯一性
		data->setName(data->getName());
		DataAddCB(data);
		return true;
	}
	
	virtual bool remove(sptr<XDataObject> where){
		if (!has(where)) {
			return false;
		}
		auto it = mDataList.begin();
		for (it = mDataList.begin(); it != mDataList.end(); ) {
			if (*it == where) {
				(*it)->setParent(nullptr);
				it = mDataList.erase(it);
			}
			else {
				++it;
			}
		}
		DataRemoveCB(where);
		return true;
	}

	int childCount() const override {
		return mDataList.size();
	}

	sptr<XDataObject> childAt(int index) const override {
		return mDataList[index];
	}

	int attrCount() const override;

	sptr<XDataAttribute> attrAt(int index) const override;

	iterator begin() {
		return mDataList.begin();
	}

	const_iterator begin() const {
		return mDataList.begin();
	}

	iterator end() {
		return mDataList.end();
	}

	const_iterator end() const {
		return mDataList.end();
	}

	size_t size() {
		return mDataList.size();
	}

	sptr<XDataObject> operator[](size_t idx) {
		return mDataList[idx];
	}

	void clear() {
		while (!mDataList.empty()) {
			remove(*mDataList.begin());
		}
	}

	virtual void serialize(HighFive::Group& group);
	virtual void deserialize(HighFive::Group& group);

protected:
	std::vector<sptr<XDataObject>> mDataList;
};
template<typename T>
class XDataListT :public XDataList {
	static_assert(std::is_base_of_v<XDataObject, T>,
		"XDataListT requires T to be derived from XDataObject");
	
	REGISTER_CLASS_META_DATA(XDataListT<T>, XDataList);
};
