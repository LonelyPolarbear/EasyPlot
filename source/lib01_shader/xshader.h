#ifndef XSHADER_HPP
#define XSHADER_HPP

#include<string>
#include <Eigen/Eigen>
#include <dataBase/dataobject.h>
#include "xshaderApi.h"

//着色器类

enum class lib01_shader_API  PrimitveType {
	point = 0x0000,						                //GL_POINTS = 0x0000;
	line = 0x0001,						                //GL_LINES = 0x0001;
	line_strip = 0x0003,				                //GL_LINE_STRIP = 0x0003;
    line_strip_adjacency= 0x000B,             //GL_LINE_STRIP_ADJACENCY 0x000B
	line_loop = 0x0002,				                //GL_LINE_LOOP = 0x0002;
	triangle = 0x0004,					                //GL_TRIANGLES = 0x0004;
	triangle_strip = 0x0005,		                //GL_TRIANGLE_STRIP = 0x0005;
	triangle_fan = 0x0006,			                //GL_TRIANGLE_FAN = 0x0006;
	quads = 0x0007,						                //GL_QUADS = 0x0007;					 核心模式中已被移除（仅兼容模式支持），不推荐使用：
	quad_strip = 0x0008,			                //GL_QUAD_STRIP = 0x0008;		 核心模式中已被移除（仅兼容模式支持），不推荐使用：
	polygon = 0x0009,					                //GL_POLYGON = 0x0009;			 核心模式中已被移除（仅兼容模式支持），不推荐使用：
};

class lib01_shader_API xshader:public DataBaseObject{
    public:
    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESS_CONTROL,
        TESS_EVALUATION,
        COMPUTE
    };
    unsigned int ID{0};

    xshader() = default;

    ~xshader();

	xshader(const std::string& vertexPath, const std::string& fragmentPath, bool isPath = true);

	xshader(std::map<ShaderType,std::string> shaderPathsOrCode, bool isPath = true);

    void create();

    unsigned int compile(ShaderType type, const std::string& shaderCode,bool isPath = true);

    void link(const std::vector<unsigned int> &shaders_id);

    void use();

    void unUse();
   
    void setBool(const std::string& name, bool value);
    
    void setInt(const std::string& name, int value);

    void setUint(const std::string& name, unsigned int value);

    void setFloat(const std::string& name, float value);

    void setVec2(const std::string& name, const Eigen::Vector2f& value);

    void setVec2(const std::string& name, float x, float y);

    void setiVec2(const std::string& name, int x, int y);

    void setVec3(const std::string& name, const Eigen::Vector3f& value);

    void setVec3(const std::string& name, float x, float y, float z);

    void setVec4(const std::string& name, const Eigen::Vector4f& value);

    void setVec4(const std::string& name, float x, float y, float z, float w);
  
    void setMat2(const std::string& name, const Eigen::Matrix2f& mat);

    void setMat3(const std::string& name, const Eigen::Matrix3f& mat);

	void setMat4(const std::string& name, const Eigen::Matrix4f& mat);

	void setMat4(const std::string& name,float*data);

    //对外高度封装的接口，这些接口对应固定的模板着色器
public:

	void setModelMatrix(float* data);

	void setModelSelfMatrix(float* data);

    void setViewMatrix(float* data);

    void setViewMatrix(const Eigen::Matrix4f& mat);

	void setProjectionMatrix(float* data);

	void setProjectionMatrix(const Eigen::Matrix4f& mat);

	void setColorMode(int mode);

	void setPolygonMode(int mode);

	void setSingleColor(float r, float g, float b, float a);

    void setFillColor(float r, float g, float b, float a);

	void setSelectTestColor(float r, float g, float b, float a);

    void setIs3D(bool is3D);

	void setScreeSize(int w, int h);

	void setRealTimeMousePos(int w, int h);

    void setPreSelectColor(float r, float g, float b, float a);

    void setObjectID(uint32_t t);

    void setLineWidth(uint32_t width);

    void setfixedLine(bool isFixed);

    void setPenStyle(int style);

    void setPositionType(int type);

    void setOrientation(int type);

    void setConenctSmoothEnable(bool enable);

    void setIsInstanceDarw(bool isInstance);

    //void setLightPos(float x, float y, float z);

    //void setLightColor(float r, float g, float b);
public:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    static void checkCompileErrors(unsigned int shader, unsigned int type);  

    static unsigned int xshader::compileShader(ShaderType type, const std::string& shaderCode);

    static xshader createFeedbackShader(unsigned int vs_id,const std::vector<std::string>& variables);

    static xshader createComputeShader(unsigned int id);
public:
};
#endif