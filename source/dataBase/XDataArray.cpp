#pragma once


#include "XDataArray.h"

template class DATABASE_API XDataArray<int>;
template class DATABASE_API XDataArray<unsigned int>;
template class DATABASE_API XDataArray<float>;
template class DATABASE_API XDataArray<double>;
template class DATABASE_API XDataArray<unsigned char>;
template class DATABASE_API XDataArray<char>;

template class DATABASE_API XDataArray2D<unsigned char>;
template class DATABASE_API XDataArray2D<char>;
template class DATABASE_API XDataArray2D<float>;
template class DATABASE_API XDataArray2D<double>;
template class DATABASE_API XDataArray2D<unsigned int>;
template class DATABASE_API XDataArray2D<int>;

template class DATABASE_API XDataArray3D<unsigned char>;
template class DATABASE_API XDataArray3D<float>;
template class DATABASE_API XDataArray3D<double>;
template class DATABASE_API XDataArray3D<unsigned int>;
template class DATABASE_API XDataArray3D<char>;
template class DATABASE_API XDataArray3D<int>;