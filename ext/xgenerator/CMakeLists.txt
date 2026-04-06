cmake_minimum_required(VERSION 3.15)

set(proName xgenerator)
project(xgenerator)


set(CMAKE_CXX_STANDARD 17)


#设置库文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG  	${CMAKE_SOURCE_DIR}/output/bin)		#设置lib目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_SOURCE_DIR}/output/bin)		#设置lib目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_SOURCE_DIR}/output/bin)		#设置dll目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_SOURCE_DIR}/output/bin)		#设置dll目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_SOURCE_DIR}/output/bin)		#设置exe目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${CMAKE_SOURCE_DIR}/output/bin)		#设置exe目录


set (CLANG_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../../3rdParty/clang_llvm)
set(Clang_DIR ${CLANG_INSTALL_DIR}/lib/cmake/clang)
set(LLVM_DIR ${CLANG_INSTALL_DIR}/lib/cmake/llvm)

file(GLOB  HEADERS_FILES src/*.h src/*.hpp)
file(GLOB SOURCES_FILES src/*.cpp)
message(STATUS "SOURCES_FILES: ${SOURCES_FILES}")
message(STATUS "HEADERS_FILES: ${HEADERS_FILES}")

find_package(Clang REQUIRED COMPONENTS clangAST clangBasic  clangFrontend  clangTooling clangRewrite )

add_executable(${proName}  ${SOURCES_FILES} ${HEADERS_FILES})
target_compile_definitions(${proName} PRIVATE REPLXX_STATIC)
target_include_directories(${proName} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CLANG_INSTALL_DIR}/include
    ${CMAKE_SOURCE_DIR}/../../3rdParty/CLI/include
    ${CMAKE_SOURCE_DIR}/../../3rdParty/linenoise/include
    ${CMAKE_SOURCE_DIR}/../../3rdParty/replxx/include
    ${CMAKE_SOURCE_DIR}/../../3rdParty/nlohmann/include
)

target_link_directories(${proName} PRIVATE 
         ${CMAKE_SOURCE_DIR}/../../3rdParty/linenoise/lib
         ${CMAKE_SOURCE_DIR}/../../3rdParty/replxx/lib/MT
)

 target_link_libraries(${proName} PRIVATE 
                clangAST
                clangBasic 
                clangFrontend
                clangTooling
                clangRewrite
                #linenoise_lib
                replxx-static-d
                )

set_target_properties(${proName} PROPERTIES
    #MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    MSVC_RUNTIME_LIBRARY "MultiThreaded"
)

if (MSVC)
    target_compile_options(${proName} PRIVATE /bigobj)
endif()