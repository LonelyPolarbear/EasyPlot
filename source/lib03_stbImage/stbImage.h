#pragma once
#include <string>
#ifdef LIB03_STBIMAGE_DLL
#define LIB03_STBIMAGE __declspec(dllexport)
#else
#define LIB03_STBIMAGE __declspec(dllimport)
#endif 

#include <string>
#include <vector>
class LIB03_STBIMAGE stbImage
{
public:
    struct imageInfo {
        int width, height, channels;
        unsigned char* data = nullptr;
    };
    static unsigned int loadTexture2D(const std::string& file);
    
    //X+ X- Y+ Y- Z+ Z-     右左 上下 前后
    static unsigned int loadCubeMap(const std::vector<std::string>& textures_faces);

	static imageInfo stbImage::readPicture(const std::string& file, bool flip);

    static void freeImageData(void * data);

};
