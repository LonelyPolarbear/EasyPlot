
#include "stbImage.h"
#include <glew/glew.h>
#define STB_DDS_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <SOIL2/SOIL2.h>
#include <SOIL2/stb_image.h>

using namespace std;
unsigned int stbImage::loadTexture2D(const std::string& file,int channel)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //设置纹理的环绕方式
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //设置纹理的环绕方式
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //设置纹理的环绕方式

    //设置纹理过滤方式
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);      //设置缩小时的过滤方式为多级渐远纹理
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                                   //设置放大时的过滤方式为双线性插值

    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, channel);

	GLenum format;
	GLenum InternalFormat = GL_RGBA;
    if (nrChannels == 1) {
        format = GL_RED;
        InternalFormat = GL_R8;
    }
	if (nrChannels == 2) {
		format = GL_RG;
		InternalFormat = GL_RG8;
	}
        
    else if (nrChannels == 3) {
        format = GL_RGB;
        InternalFormat = GL_RGB8;
    }
        
    else if (nrChannels == 4) {
        format = GL_RGBA;
        InternalFormat = GL_RGBA8;
    }
       

    if (data)
    {
		/*auto num = width * height * nrChannels;
		std::vector<unsigned char> dd;
		dd.resize(num);
		memcpy(dd.data(), data, num);*/

        glTexStorage2D(GL_TEXTURE_2D,1, InternalFormat,width,height);

		auto alignment = 1;
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		//auto oldPackAlignment = XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, alignment);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

		//std::vector<unsigned char> dd2;
		//dd2.resize(num);

  //      glPixelStorei(GL_PACK_ALIGNMENT, alignment);
		//glGetTexImage(
  //          GL_TEXTURE_2D,							// 纹理目标
		//	0,											// mipmap级别
  //          format,					// 格式（必须匹配纹理的内部格式）
  //          GL_UNSIGNED_BYTE,						// 类型（必须匹配纹理的数据类型）
		//	dd2.data()										// 偏移量（使用PBO时设为0）
		//);



		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			int i=0;
		}
		
		
		//XOpenGLFuntion::xglPixelStorei(XOpenGL::PixelStoreParameter::unpack_alignment, oldPackAlignment);
		//XOpenGLFuntion::checkGLError();
        //glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
       // glGenerateMipmap(GL_TEXTURE_2D); //让系统自动处理多级渐远纹理
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);      //释放内存
    return texture;
}

unsigned int stbImage::LoadSMAATexture(const std::string& file,int channel)
{
    int w,h,c;
    unsigned char* data = SOIL_load_image(file.c_str(), &w, &h, &c, channel);
    if (data) {
		auto num = w * h * c;
		std::vector<unsigned char> dd;
		dd.resize(num);
		memcpy(dd.data(), data, num);
    }
   
	GLuint tex = SOIL_load_OGL_texture(
        file.c_str(),
        channel,
		SOIL_CREATE_NEW_ID,
        SOIL_FLAG_DDS_LOAD_DIRECT);
	if (tex == 0) {
		// 加载失败，打印 SOIL2 内部的错误信息
		printf("DDS 纹理加载失败: %s\n", SOIL_last_result());
	}
	else {
		printf("DDS 纹理加载成功，ID: %u\n", tex);
	}
   // SOIL_free_image_data(data);
	return tex;
}

stbImage::imageInfo stbImage::readPicture(const std::string& file,bool flip) {
	int width, height, nrChannels;
    imageInfo info;
	stbi_set_flip_vertically_on_load(flip);
	info.data = stbi_load(file.c_str(), &info.width, &info.height, &info.channels, 0);
    return info;
}

void stbImage::freeImageData(void* data)
{
    if(data)
        stbi_image_free(data);
}

//X+ X- Y+ Y- Z+ Z-     右左 上下 前后
unsigned int stbImage::loadCubeMap(const std::vector<std::string>& textures_faces)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texture;
}
