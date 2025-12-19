#pragma once


#include "XDataArray.h"

template class database_API XDataArray<int>;
template class database_API XDataArray<unsigned int>;
template class database_API XDataArray<float>;
template class database_API XDataArray<double>;
template class database_API XDataArray<unsigned char>;
template class database_API XDataArray<char>;

template class database_API XDataArray2D<unsigned char>;
template class database_API XDataArray2D<char>;
template class database_API XDataArray2D<float>;
template class database_API XDataArray2D<double>;
template class database_API XDataArray2D<unsigned int>;
template class database_API XDataArray2D<int>;

template class database_API XDataArray3D<unsigned char>;
template class database_API XDataArray3D<float>;
template class database_API XDataArray3D<double>;
template class database_API XDataArray3D<unsigned int>;
template class database_API XDataArray3D<char>;
template class database_API XDataArray3D<int>;