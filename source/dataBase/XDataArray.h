#pragma once
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include "dataobject.h"
#include "databaseApi.h"
#include "XTimeStamp.h"
/// <summary>
/// 一维数组
/// </summary>
template<typename T>
class XDataArray : public DataBaseObject
{
 protected:
    XDataArray():DataBaseObject(){}
    XDataArray( int tupleNum,int componentNum=1) :DataBaseObject() {
        setComponent(componentNum);
        setNumOfTuple(tupleNum);
    }
    ~XDataArray(){}
public:
     T* data(int tupleIdx)  {
        return d.data()+tupleIdx*component;
    }

     size_t size() const {
        return d.size();
    }
     
     size_t bytesNum() const{
         return d.size()*sizeof(T);
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

    //单纯内存拷贝
    template<typename U>
	void memCopy(U* srcPtr, int numTuple) {
		auto copyBytenNum = std::min<int>(numTuple*sizeof(U), bytesNum());
		memcpy(d.data(), srcPtr, copyBytenNum);
	}

    template<typename U>
    void memCopy(std::shared_ptr< XDataArray<U>> src) {
        if(!src)
            return ;
        auto copyBytenNum = std::min<int>(src->bytesNum(), bytesNum());
        memcpy(d.data(),src->data(0), copyBytenNum);
    }

    void setName(const std::string& n) {
        name = n;
    }

    std::string getName() const {
        return name;
    }


	void dump() {
		for (int i = 0; i < d.size(); i++) {
			if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
				std::cout << (int)d[i] << " ";
			}
			else {
				std::cout << d[i] << " ";
			}
		}
		std::cout << std::endl;
	}

    /// <summary>
    /// 数据不变，数据类型转型
    /// </summary>
    template<typename U>
    sptr<XDataArray<U>> heteroCast(int componentNum) {
        auto num = bytesNum()/sizeof(U);//取整
        auto numTuple = num/componentNum;   //进一步取整

        auto dest =makeShareDbObject<XDataArray<U>>();
        dest->setComponent(componentNum);
        dest->setNumOfTuple(numTuple);

        auto copyByteNum = std::min(bytesNum(), dest->bytesNum());
        memcpy(dest->data(0),data(0),copyByteNum);
        return dest;
    }
    
    #if 0
	template<typename U>
	sptr<XDataArray<U>> heteroCast() {
		auto tupleNum = getNumOfTuple();
		auto typeNum = size();
		auto dest = makeShareDbObject<XDataArray<U>>();
		dest->setComponent(component);
		dest->setNumOfTuple(tupleNum);

		auto pdest = dest->data(0);
		auto src = data(0);
		for (int i = 0; i < typeNum; i++) {
			pdest[i] = src[i];
		}
		return dest;
	}
    #endif
	template<typename ...Args>
	void setRowData(Args&& ...args) {
		setRowDataImpl(std::make_index_sequence<sizeof...(Args)>{}, std::forward_as_tuple(args...));
	}

private:
	template<size_t...Is, typename Tuple>
	void setRowDataImpl(std::index_sequence<Is...>, Tuple&& args) {
		auto dest = data(0);
		((dest[Is] = std::get<Is>(std::forward<Tuple>(args))), ...);
	}
private:
    std::vector<T>  d;
    unsigned int component =1;
    std::string name;
};

extern  template class database_API XDataArray<int>;
extern  template class database_API XDataArray<unsigned int>;
extern  template class database_API XDataArray<float>;
extern  template class database_API XDataArray<double>;
extern  template class database_API XDataArray<unsigned char>;
extern  template class database_API XDataArray<char>;
using XIntArray = XDataArray<int>;
using XUIntArray = XDataArray<unsigned int>;
using XFloatArray = XDataArray<float>;
using XDoubleArray = XDataArray<double>;
using XUCharArray = XDataArray<unsigned char>;
using XCharArray = XDataArray<char>;

/// <summary>
/// 二维数组
/// </summary>
template<typename T>
class XDataArray2D : public DataBaseObject
{
 protected:
    XDataArray2D() {
        mData = makeShareDbObject<XDataArray<T>>();
    }

	XDataArray2D(int width, int height,int component=1 ) {
		mData = makeShareDbObject<XDataArray<T>>();
		setComponent(component);
        setDimensions(width,height);
	}
    ~XDataArray2D() {}
public:
	void setName(const std::string& n) {
		mData->setName(n);
	}

    void setComponent(unsigned int com) {
        mData->setComponent(com);
    }

    void setDimensions(unsigned int width, unsigned int height) {
        mRow = height;
        mCol = width;
        mData->setNumOfTuple(height * width);
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

    T* data(int row=0, int col=0) {
        return mData->data(row*mCol + col);
    }

    size_t size() const {
        return mData->size();
    }

	size_t bytesNum() const {
		return mData->bytesNum();
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
        mData->setTuple(y * mCol + x, args...);
	}

    template<typename U>
    void memCopy(sptr<XDataArray<U>> inputData) {
        if(!inputData)
            return ;

        auto copyBytenum = std::min(inputData->bytesNum(), bytesNum());
        auto dest = data(0,0);
        auto src = inputData->data(0);
        memcpy(dest, src, copyBytenum);
    }

    template<typename ...Args>
    void setRowData(Args&& ...args) {
        setRowDataImpl(std::make_index_sequence<sizeof...(Args)>{}, std::forward_as_tuple(args...));
    }

private:
	template<size_t...Is, typename Tuple>
	void setRowDataImpl(std::index_sequence<Is...>, Tuple&& args) {
        auto dest = data(0, 0);
		((dest[Is] = std::get<Is>(std::forward<Tuple>(args))), ...);
	}
 public:

    /// <summary>
    /// 克隆一个区域数据
    /// </summary>
    std::shared_ptr<XDataArray2D> subRect(int x, int y, int width, int height) {
		width = std::min<int>(getCol() - x, width);
        height = std::min<int>(getRow() - y, height);

        std::shared_ptr<XDataArray2D> sub = makeShareDbObject<XDataArray2D>();
        sub->setDimensions(width,height);
        sub->setComponent(mData->getComponent());
        //拷贝数据，逐行拷贝
        for (int i = 0; i < height; i++) {
            auto curRow = data(y + i, x);
            auto curSubRow = sub->data(i,0);
            memcpy(curSubRow,curRow, mData->getComponent()*sizeof(T)*width);
        }
        return sub;
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

    void dump(bool reverse = false /*是否逆序*/) {
        std::cout<<"width:"<<mCol <<" height:"<<mRow<<std::endl;
        for (int row = 0; row < mRow; row++) {
            auto curRow = reverse ? mRow - row-1 :row ;
            for (int col = 0; col < mCol; col++) {
                auto ptr = mData->data(curRow * mCol + col);
				if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
					for (int c = 0; c < getComponent(); c++) {
                        std::cout << (int)ptr[c] << " ";
					}
				}
				else {
					for (int c = 0; c < getComponent(); c++) {
						std::cout << ptr[c] << " ";
					}
				}  
            }
            std::cout<<std::endl;
        }
        std::cout << std::endl;
    }

	/// <summary>
   /// 数据不变，数据类型转型
   /// </summary>
	template<typename U>
	sptr<XDataArray2D<U>> heteroCast(int w, int componentNum) {
		auto num = bytesNum() / sizeof(U);//取整
		auto numTuple = num / componentNum;   //进一步取整

        //numTuple 每行w个
        auto h= numTuple /w;

		auto dest = makeShareDbObject<XDataArray2D<U>>();
		dest->setComponent(componentNum);
        dest->setDimensions(w,h);

		auto copyByteNum = std::min(bytesNum(), dest->bytesNum());
		memcpy(dest->data(0), data(0), copyByteNum);
		return dest;
	}

    #if 0
	template<typename U>
	sptr<XDataArray2D<U>> heteroCast() {
		auto tupleNum = getNumOfTuple();
		auto typeNum = size();
		auto dest = makeShareDbObject<XDataArray2D<U>>();
		dest->setComponent(component);
		dest->setDimensions(mCol,mRow);

		auto pdest = dest->data(0);
		auto src = data(0);
		for (int i = 0; i < typeNum; i++) {
			pdest[i] = src[i];
		}
		return dest;
	}
    #endif

private:
    std::shared_ptr<XDataArray<T>> mData;        //实际存储的数据
 private:
    unsigned int mRow = 0;  //行数，从上到下递增
    unsigned int mCol = 0;    //列数，从左到右递增
    //主要设计用于存储图像数据，默认情况下，图像数据是行优先存储的，即一行一行的存储，每一行的长度为列数
 };

 extern  template class database_API XDataArray2D<unsigned char>;
 extern  template class database_API XDataArray2D<float>;
 extern  template class database_API XDataArray2D<double>;
 extern  template class database_API XDataArray2D<unsigned int>;
 extern  template class database_API XDataArray2D<int>;
 extern  template class database_API XDataArray2D<char>;
 using XUCharArray2D = XDataArray2D<unsigned char>;
 using XUIntArray2D = XDataArray2D<unsigned int>;
 using XIntArray2D = XDataArray2D<int>;
 using XFloatArray2D = XDataArray2D<float>;
 using XDoubleArray2D = XDataArray2D<double>;
 using XCharArray2D = XDataArray2D<char>;

 /// <summary>
/// 三维数组
/// </summary>
 template<typename T>
 class XDataArray3D : public DataBaseObject
 {
 protected:
     XDataArray3D() {
		 mData = makeShareDbObject<XDataArray<T>>();
	 }

	 XDataArray3D(int width,int height,int zlen,int component=1) {
		 mData = makeShareDbObject<XDataArray<T>>();
         setComponent(1);
         setDimensions(width,height,zlen);
	 }

	 ~XDataArray3D() {}
public:
	void setName(const std::string& n) {
		mData->setName(n);
	}

	void setComponent(unsigned int com) {
		mData->setComponent(com);
	}

	void setDimensions( unsigned int width, unsigned int height,unsigned int depth) {
		mRow = height;
		mCol = width;
        mLen = depth;
		mData->setNumOfTuple(height * width*depth);
	}

	unsigned int getComponent() const {return mData->getComponent();}

	unsigned int getRow() const {return mRow;}

	unsigned int getCol() const {return mCol;}

	unsigned int getDepth() const {return mLen;}

	size_t size() const {
		return mData->size();
	}

	size_t bytesNum() const {
		return mData->bytesNum();
	}

	T* data(int sliceIdx=0/*切片索引*/, int x =0, int y =0) {
        auto col =x;
        auto row = y;
		return mData->data(sliceIdx*mRow*mCol+ row * mCol + col);
	}
    
	template<typename ... types>
	void setData(int z,int x, int y, types... args) {
		mData->setTuple(z*mRow*mCol+  y * mCol + x, args...);
	}

	void dump(bool reverse = false /*是否逆序*/) {
        std::cout<<"width:"<<mCol<<" height:"<<mRow<<" z:"<<mLen<<std::endl;
        for (int z = 0; z < mLen; z++) {
            std::cout<<"z="<<z<<std::endl;
			for (int row = 0; row < mRow; row++) {
				auto curRow = reverse ? mRow - row - 1 : row;
				for (int col = 0; col < mCol; col++) {
					auto ptr = mData->data(z * mRow * mCol + curRow * mCol + col);
					if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
						for (int c = 0; c < getComponent(); c++) {
							std::cout << (int)ptr[c] << " ";
						}
					}
					else {
						for (int c = 0; c < getComponent(); c++) {
							std::cout << ptr[c] << " ";
						}
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
        }
        std::cout << std::endl;
	}

    template<typename U>
    void memCopy(int z, std::shared_ptr< XDataArray2D<U>> sliceData) {
        if(!sliceData)
            return;
   
        auto copyByteNum = std::min(mRow * mCol * getComponent() * sizeof(T), sliceData->bytesNum());

        auto dst =data(z,0,0);
        auto src = sliceData->data(0,0);
        //将当前切片的数据拷贝，只复制拷贝，外部控制对齐
        memcpy(dst,src, copyByteNum);
    }

    template<typename U>
	void memCopy(sptr< XDataArray<U>> InputData) {
		if (!InputData)
			return;

        auto copyByteNum= std::min(InputData->bytesNum(), bytesNum());

		auto dst = data(0, 0, 0);
		auto src = InputData->data(0);
		//将当前切片的数据拷贝，只复制拷贝，外部控制对齐
		memcpy(dst, src, copyByteNum);
	}

    std::shared_ptr< XDataArray2D<T>> getData(int z = 0) {
        auto ret = makeShareDbObject<XDataArray2D<T>>();
        ret->setDimensions(mCol,mRow);
        ret->setComponent(getComponent());
        if (z <= mLen) {
			auto dst = ret->data(0, 0);
			auto src = data(z,0, 0);
            memcpy(dst, src, mRow * mCol * sizeof(T));
        }
        return ret;
    }

	void forEach(std::function<void(T* d)> f) {
		for (int i = 0; i < mData->getNumOfTuple(); i++) {
			auto ptr = mData->data(i);
			f(ptr);
		}
	}

	void forEach(int z,std::function<void(T* d)> f) {
        auto start = z*mCol*mRow;
        auto end = start+mCol*mRow;
		for (int i = start; i < end; i++) {
			auto ptr = mData->data(i);
			f(ptr);
		}
	}

	/// <summary>
   /// 数据不变，数据类型转型
   /// </summary>
	template<typename U>
	sptr<XDataArray3D<U>> heteroCast(int w,int z, int componentNum) {
		auto num = bytesNum() / sizeof(U);//取整
		auto numTuple = num / componentNum;   //进一步取整

		auto pageNum = numTuple / z;
        auto h = pageNum/w;

		auto dest = makeShareDbObject<XDataArray3D<U>>();
		dest->setComponent(componentNum);

		dest->setDimensions(w,h,z);

		auto copyByteNum = std::min(bytesNum(), dest->bytesNum());
		memcpy(dest->data(0), data(0), copyByteNum);
		return dest;
	}

    #if 0
	template<typename U>
	sptr<XDataArray3D<U>> heteroCast() {
		auto typeNum = size();
		auto dest = makeShareDbObject<XDataArray3D<U>>();
		dest->setComponent(component);
		dest->setDimensions(mCol, mRow,mLen);

		auto pdest = dest->data(0);
		auto src = data(0);
		for (int i = 0; i < typeNum; i++) {
			pdest[i] = src[i];
		}
		return dest;
	}
    #endif
	template<typename ...Args>
	void setRowData(Args&& ...args) {
		setRowDataImpl(std::make_index_sequence<sizeof...(Args)>{}, std::forward_as_tuple(args...));
	}

private:
	template<size_t...Is, typename Tuple>
	void setRowDataImpl(std::index_sequence<Is...>, Tuple&& args) {
		auto dest = data(0);
		((dest[Is] = std::get<Is>(std::forward<Tuple>(args))), ...);
	}
 private:
	 std::shared_ptr<XDataArray<T>> mData;        //实际存储的数据
 private:
	 unsigned int mRow = 0;         //行数，从上到下递增    高
	 unsigned int mCol = 0;           //列数，从左到右递增    宽
     unsigned int mLen =0;           //长度
  };

extern  template class database_API XDataArray3D<unsigned char>;
extern  template class database_API XDataArray3D<float>;
extern  template class database_API XDataArray3D<double>;
extern  template class database_API XDataArray3D<unsigned int>;
extern  template class database_API XDataArray3D<char>;
extern  template class database_API XDataArray3D<int>;

using XUCharArray3D = XDataArray3D<unsigned char>;
using XUIntArray3D = XDataArray3D<unsigned int>;
using XIntArray3D = XDataArray3D<int>;
using XFloatArray3D = XDataArray3D<float>;
using XDoubleArray3D = XDataArray3D<double>;
using XCharArray3D = XDataArray3D<char>;