#include <fstream>
#include <sstream>
#include <iostream>
#include <glew/glew.h>
#include "xshader.h"

static std::string readFile(const std::string& path) {
    std::ifstream ShaderFile;
    ShaderFile.open(path);

    std::stringstream ShaderStream;
    // read file's buffer contents into streams
    ShaderStream << ShaderFile.rdbuf();

    ShaderFile.close();
    return ShaderStream.str();
}
xshader::~xshader()
{
    //glDeleteShader(ID);
}

xshader::xshader(const std::string& vertexPath, const std::string& fragmentPath,bool isPath)
{
    //!
    //![1]
    std::string vertexCode; 
    std::string fragmentCode;
    if (isPath) {
        vertexCode = readFile(vertexPath);
        fragmentCode = readFile(fragmentPath);
    }
    else {
        vertexCode = vertexPath;
        fragmentCode = fragmentPath;
    }

    //!
    //![2] compile shaders
    unsigned int vertex, fragment;
    vertex = compileShader(ShaderType::VERTEX, vertexCode);
    fragment = compileShader(ShaderType::FRAGMENT, fragmentCode);
    
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, GL_PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

xshader::xshader(std::map<ShaderType, std::string> shaderPathsOrCode, bool isPath)
{
	std::map<ShaderType, std::string> shaderCode;
	if (!isPath) {
		shaderCode = shaderPathsOrCode;
	}
	std::vector<unsigned int> shaders_id;
	for (auto& [type, path] : shaderPathsOrCode) {
		std::string code = readFile(path);
		shaderCode[type] = std::move(code);
	}

	ID = glCreateProgram();

	for (auto& [type, code] : shaderCode) {
		auto shader_id = compileShader(type, code);
		shaders_id.push_back(shader_id);
		glAttachShader(ID, shader_id);
	}

	glLinkProgram(ID);
	checkCompileErrors(ID, GL_PROGRAM);
	for (auto id : shaders_id) {
		glDeleteShader(id);
	}
}

void xshader::setModelMatrix(float* data)
{
    setMat4("ModelMat",data);
}

void xshader::setModelSelfMatrix(float* data)
{
    setMat4("ModelMatSelf", data);
}

void xshader::setViewMatrix(float* data)
{
    setMat4("ViewMat", data);
}

void xshader::setViewMatrix(const Eigen::Matrix4f& mat)
{
    setMat4("ViewMat", mat);
}

void xshader::setProjectionMatrix(float* data)
{
    setMat4("ProjectionMat", data);
}

void xshader::setProjectionMatrix(const Eigen::Matrix4f& mat)
{
    setMat4("ProjectionMat", mat);
}

void  xshader::setColorMode(int mode) {
    setInt("colorMode",mode);
}

void xshader::setPolygonMode(int mode)
{
    setInt("polygonMode", mode);
}

void xshader::setSingleColor(float r, float g, float b, float a) {
    setVec4("singleColor",Eigen::Vector4f(r,g,b,a));
}

void xshader::setFillColor(float r, float g, float b, float a)
{
    setVec4("fillColor", Eigen::Vector4f(r, g, b, a));
}

void xshader::setSelectTestColor(float r, float g, float b, float a)
{
	setVec4("selectTestColor", Eigen::Vector4f(r, g, b, a));
}

void xshader::setIs3D(bool is3D)
{
    setBool( "is3D", is3D);
}

void xshader::setScreeSize(int w, int h)
{
    setVec2("screenSize",w,h);
}

void xshader::setRealTimeMousePos(int w, int h)
{
    setVec2("mousePos", w, h);
}

void xshader::setPreSelectColor(float r, float g, float b, float a)
{
    setVec4("preSelectColor", Eigen::Vector4f(r, g, b, a));
}

void xshader::setObjectID(uint32_t t)
{
    setUint("objectID",t);
}

void xshader::setLineWidth(uint32_t width)
{
    setUint("lineWidth", width);
}

void xshader::setfixedLine(bool isFixed) {
    setBool("isFixedLine", isFixed);
}

void xshader::setPenStyle(int style)
{
    setInt("penStyle",style);
}

void xshader::setPositionType(int type)
{
    setInt("PositionType", type);
}

void xshader::setOrientation(int type)
{
    setInt("Orientation", type);
}

void xshader::setConenctSmoothEnable(bool enable)
{
    setBool("connectSmoothEnable", enable);
}

void xshader::setIsInstanceDarw(bool isInstance) {
    setBool("isInstance",isInstance);
}

void xshader::textureBinding( const std::string& name, int bindingPoint)
{
    setInt(name,bindingPoint);
}

void xshader::uboBinding( const std::string& name, int bindingPoint)
{
	GLuint uboIndex = glGetUniformBlockIndex(ID, name.c_str());
	glUniformBlockBinding(ID, uboIndex, bindingPoint);
}

void xshader::ssboBinding(const std::string& name,int bindingPoint )
{
	GLuint ssboIndex = glGetProgramResourceIndex(ID, GL_SHADER_STORAGE_BLOCK, name.c_str());
	glShaderStorageBlockBinding(ID, ssboIndex, bindingPoint);
}