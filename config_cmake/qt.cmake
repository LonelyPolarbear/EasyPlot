#SET Qt5_DIR=C:\Qt\5.15.2\msvc2019_64

#-------------------------------------------------------
#Qt支持
#-------------------------------------------------------

#set(Qt5_DIR C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5)
# set(CMAKE_PREFIX_PATH C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5 ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH C:/Qt/5.15.2/msvc2019_64 ${CMAKE_PREFIX_PATH})
#这些生成的中间文件都会在build目录下，这样的话，编译器则不能定位由uic程序产生的诸如_ui_mainwindow.h等文件。所以，我们需要把build目录添加到包含目录中
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
#Qt5Config.cmake 的位置C:\Qt\5.15.2\mingw81_64\lib\cmake\Qt5
#set(CMAKE_PREFIX_PATH "C:/Qt\5.15.2/msvc2019_64/bin")
#set(Qt5_DIR C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
#list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "./")
