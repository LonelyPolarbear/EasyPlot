#pragma once
#include <functional>
#include <vector>
#include <string>
#include "dataobject.h"
#include "databaseApi.h"
#include "XTimeStamp.h"

template<typename T>
class XDataArray : public DataBaseObject
{
 protected:
    XDataArray():DataBaseObject(){}
    ~XDataArray(){}
public:
     T* data(int tupleIdx)  {
        return d.data()+tupleIdx*component;
    }

     size_t size() const {
        return d.size();
    }

     void setNumOfTuple(int size) {
        d.resize(size* component,0);
    }

    void setComponent(unsigned int c) {
        component = c;
    }

    unsigned int getComponent() const {
        return component;
    }

    int getNumOfTuple() const {
        return d.size() / component;
    }

    template<typename ... types>
    void setTuple(int tupleIdx, types... args) {
        constexpr int numOfArgs = sizeof...(types);
        T* ptr = data(tupleIdx);
       int i=0;
        ((ptr[i++] = args), ...);
    }


    void clear() {
        d.clear();
    }

    bool deepCopy(std::shared_ptr< XDataArray<T>> src) {
        if ( src->size() ==0) {
            return false;
        }

        if(src->getComponent() !=component)
            return false;

        d.resize(src->size());
        memcpy(d.data(),src->data(0),src->size()*sizeof(T));
        Modified();     //标识当前数据已经更改
        return true;
    }

    void setName(const std::string& n) {
        name = n;
    }

    std::string getName() const {
        return name;
    }
    
private:
    std::vector<T>  d;
    unsigned int component =1;
    std::string name;
};

template<typename T>
class XDataArray2D : public DataBaseObject
{
 protected:
    XDataArray2D() {
        mData = makeShareDbObject<XDataArray<T>>();
    }
    ~XDataArray2D() {}
public:
	void setName(const std::string& n) {
		mData->setName(n);
	}

    void setComponent(unsigned int com) {
        mData->setComponent(com);
    }

    void setDimensions(unsigned int row, unsigned int col) {
        mRow = row;
        mCol = col;
        mData->setNumOfTuple(row*col);
    }

    unsigned int getComponent() const {
        return mData->getComponent();
    }

    unsigned int getRow() const {
        return mRow;
    }

    unsigned int getCol() const {
        return mCol;
    }

    T* data(int row, int col) {
        return mData->data(row*mCol + col);
    }

    size_t size() const {
        return mData->size();
    }

    void clear() {
        mData->clear();
    }

	std::string getName() const {
		return mData->getName();
	}

    void Modified() {
        DataBaseObject::Modified();
    }

	template<typename ... types>
	void setData(int x,int y,types... args) {
        mData->setTuple(x * mCol + y, args...);
	}

    //设置矩形区域的数据
    //x,y为矩形左下角坐标，w,h为矩形的宽和高

    void setData(int x, int y, int w, int h, T* value) {
        //逐行拷贝
        auto component = mData->getComponent();
        for (int i = 0; i < h; i++) {
            memcpy(data(y+i, x), value + i * w* component, w * sizeof(T)* component);
        }
    }

    void forEach(std::function<void(T* d)> f) {
        for (int i = 0; i < mData->getNumOfTuple(); i++) {
                auto ptr = mData->data(i);
            f(ptr);
        }
    }

    void flip() {
        //翻转数据，上下颠倒
        //创建一个新的数组，然后逐行拷贝
        auto component = mData->getComponent();
        auto row = mRow;
        auto col = mCol;
        auto size = mData->size();

        auto newData = makeShareDbObject<XDataArray<T>>();
        newData->setComponent(component);
        newData->setNumOfTuple(row*col);
       
        for (int i = 0; i < row; i++) {
            auto src = mData->data(i * col);
            auto dst = newData->data((row - 1 - i) * col);
            memcpy(dst, src, col * sizeof(T)* component);
        }
        mData = newData;
        Modified();
    }
private:
    std::shared_ptr<XDataArray<T>> mData;        //实际存储的数据
 private:
    unsigned int mRow = 0;  //行数，从上到下递增
    unsigned int mCol = 0;    //列数，从左到右递增
    //bool isFlip = false;            //是否翻转，不翻转是行优先存储，从上到下，从左到右，翻转是是从下到上，从左到右
    //主要设计用于存储图像数据，默认情况下，图像数据是行优先存储的，即一行一行的存储，每一行的长度为列数
 };

extern  template class database_API XDataArray<int>;
extern  template class database_API XDataArray<unsigned int>;
extern  template class database_API XDataArray<float>;
extern  template class database_API XDataArray<double>;
extern  template class database_API XDataArray<unsigned char>;
extern  template class database_API XDataArray<char>;
extern  template class database_API XDataArray2D<unsigned char>;
extern  template class database_API XDataArray2D<float>;
extern  template class database_API XDataArray2D<double>;
extern  template class database_API XDataArray2D<unsigned int>;

using XIntArray = XDataArray<int>;
using XUIntArray = XDataArray<unsigned int>;
using XFloatArray = XDataArray<float>;
using XDoubleArray = XDataArray<double>;
using XUCharArray = XDataArray<unsigned char>;
using XCharArray = XDataArray<char>;
using XUCharArray2D = XDataArray2D<unsigned char>;
using XUIntArray2D = XDataArray2D<unsigned int>;
using XFloatArray2D = XDataArray2D<float>;
using XDoubleArray2D = XDataArray2D<double>;
using XCharArray2D = XDataArray2D<char>;
