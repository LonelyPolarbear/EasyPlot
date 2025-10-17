#include <fstream>
#include <sstream>
#include <iostream>
#include <glew/glew.h>
#include "xshader.h"
std::string readFile(const std::string& path) {
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

void xshader::create()
{
    ID = glCreateProgram();
}

unsigned int xshader::compile(ShaderType type, const std::string& shaderPath,  bool isPath)
{
   auto  shaderCode = isPath ? readFile(shaderPath):shaderPath;
   auto  shaderId = compileShader(type, shaderCode);
   return shaderId;
}

void xshader::link(const std::vector<unsigned int>& shaders_id)
{
    for (auto id : shaders_id) {
        glAttachShader(ID, id);
    }

	glLinkProgram(ID);
	checkCompileErrors(ID, GL_PROGRAM);
	for (auto id : shaders_id) {
		glDeleteShader(id);
	}
}

void xshader::use()
{
    glUseProgram(ID);
}

void xshader::unUse()
{
    glUseProgram(0);
}

void xshader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation((GLint)ID, name.c_str()), (int)value);
}

void xshader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void xshader::setUint(const std::string& name, unsigned int value)
{
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

void xshader::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void xshader::setVec2(const std::string& name, const Eigen::Vector2f& value)
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void xshader::setVec2(const std::string& name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void xshader::setiVec2(const std::string& name, int x, int y)
{
    glUniform2i(glGetUniformLocation(ID, name.c_str()), x, y);
}

void xshader::setVec3(const std::string& name, const Eigen::Vector3f& value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void xshader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void xshader::setVec4(const std::string& name, const Eigen::Vector4f& value)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void xshader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void xshader::setMat2(const std::string& name, const Eigen::Matrix2f& mat)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void xshader::setMat3(const std::string& name, const Eigen::Matrix3f& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void xshader::setMat4(const std::string& name, const Eigen::Matrix4f& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void xshader::setMat4(const std::string& name, float* data)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, data);
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

void xshader::checkCompileErrors(unsigned int shader, unsigned int type)
{
    std::map<GLuint, std::string> map{
        {GL_PROGRAM,"GL_PROGRAM"},
        {GL_VERTEX_SHADER,"VertexShader"},
        {GL_FRAGMENT_SHADER,"FragmentShader"},
        {GL_GEOMETRY_SHADER,"GeometryShader"},
        {GL_TESS_CONTROL_SHADER,"TessControlShader"},
        {GL_TESS_EVALUATION_SHADER,"TessEvaluationShader"}
    };
    int success;
    char infoLog[1024];
    if (type != GL_PROGRAM)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << map[type] << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << map[type] << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

unsigned int xshader::compileShader(ShaderType type , const std::string& shaderCode)
{
    const char* source = shaderCode.c_str();
    unsigned int glType;
    switch (type)
    {
    case ShaderType::VERTEX:
        glType = GL_VERTEX_SHADER;
        break;
    case ShaderType::FRAGMENT:  
		glType = GL_FRAGMENT_SHADER;
		break;    
    case ShaderType::GEOMETRY:          
		glType = GL_GEOMETRY_SHADER;
		break;
    case ShaderType::TESS_CONTROL:          
		glType = GL_TESS_CONTROL_SHADER;
		break;
    case ShaderType::TESS_EVALUATION:        
		glType = GL_TESS_EVALUATION_SHADER;
		break;
    }
    

    GLuint shader = glCreateShader(glType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    checkCompileErrors(shader, glType);
    return shader;

}

xshader xshader::createFeedbackShader(unsigned int vs_id, const std::vector<std::string>& variables)
{
    xshader shader;
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, vs_id);

    std::vector<const char*> d;
    auto str = variables;
    for (auto& t : str) {
        d.push_back(t.c_str());
    }

    glTransformFeedbackVaryings(shader.ID, variables.size(), d.data(), GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(shader.ID);
      
    checkCompileErrors(shader.ID, GL_PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vs_id);
    
    return shader;
}

xshader xshader::createComputeShader(unsigned int id)
{
    xshader shader;
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, id);
    glLinkProgram(shader.ID);

    checkCompileErrors(shader.ID, GL_PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(id);

    return shader;
}
