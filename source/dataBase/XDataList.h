#include "XDataObject.h"
#include "databaseApi.h"
#include <type_traits>

template<typename T>
class XDataListT :public XDataObject {
	static_assert(std::is_base_of_v<XDataObject, T>,
		"XDataListT requires T to be derived from XDataObject");
	using iterator = typename std::vector<sptr<T>>::iterator;
	using const_iterator = typename std::vector<sptr<T>>::const_iterator;

	REGISTER_CLASS_META_DATA(XDataListT<T>, XDataObject);
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

	bool has(sptr<T> data) {
		for (auto t : mDataList) {
			if (t == data) 
				return true;
		}
		return false;
	}

	bool push_back(sptr<T> data) {

		if (has(data)) {
			return false ;
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

	iterator erase(const_iterator where) {
		if (!has(*where)) {
			return mDataList.end();
		}
		auto it = mDataList.begin();
		for (it = mDataList.begin(); it != mDataList.end(); ) {
			if (*it == *where) {
				(*it)->setParent(nullptr);
				it = mDataList.erase(it);
			}
			else {
				++it;
			}
		}
		DataRemoveCB(*where);
		return it;
	}

	void clear() {
		mDataList.clear();
	}

	size_t size() {
		return mDataList.size();
	}

	sptr<T> operator[](size_t idx) {
		return mDataList[idx];
	}

	sptr<const T>& operator[](size_t idx) const {
		return mDataList[idx];
	}

	int childCount() const override {
		return mDataList.size();
	}
	sptr<XDataObject> childAt(int index) const override {
		return mDataList[index];
	}
protected:
	std::vector<sptr<T>> mDataList;
};
