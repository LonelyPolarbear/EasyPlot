#include <fstream>
#include <sstream>
#include <iostream>
#include <glew/glew.h>
#include "XBaseShader.h"
std::string readFile(const std::string& path) {
    std::ifstream ShaderFile;
    ShaderFile.open(path);

    std::stringstream ShaderStream;
    // read file's buffer contents into streams
    ShaderStream << ShaderFile.rdbuf();

    ShaderFile.close();
    return ShaderStream.str();
}
XBaseShader::~XBaseShader()
{
    glDeleteShader(ID);
}

void XBaseShader::create()
{
    ID = glCreateProgram();
}

unsigned int XBaseShader::compile(ShaderType type, const std::string& shaderPath,  bool isPath)
{
   auto  shaderCode = isPath ? readFile(shaderPath):shaderPath;
   auto  shaderId = compileShader(type, shaderCode);
   return shaderId;
}

void XBaseShader::link(const std::vector<unsigned int>& shaders_id)
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

void XBaseShader::use()
{
    glUseProgram(ID);
}

void XBaseShader::unUse()
{
    glUseProgram(0);
}

void XBaseShader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation((GLint)ID, name.c_str()), (int)value);
}

void XBaseShader::setInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void XBaseShader::setUint(const std::string& name, unsigned int value)
{
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

void XBaseShader::setFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void XBaseShader::setVec2(const std::string& name, const Eigen::Vector2f& value)
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void XBaseShader::setVec2(const std::string& name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void XBaseShader::setiVec2(const std::string& name, int x, int y)
{
    glUniform2i(glGetUniformLocation(ID, name.c_str()), x, y);
}

void XBaseShader::setVec3(const std::string& name, const Eigen::Vector3f& value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void XBaseShader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void XBaseShader::setVec4(const std::string& name, const Eigen::Vector4f& value)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value.data());
}

void XBaseShader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void XBaseShader::setMat2(const std::string& name, const Eigen::Matrix2f& mat)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void XBaseShader::setMat3(const std::string& name, const Eigen::Matrix3f& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void XBaseShader::setMat4(const std::string& name, const Eigen::Matrix4f& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.data());
}

void XBaseShader::setMat4(const std::string& name, float* data)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, data);
}

void XBaseShader::checkCompileErrors(unsigned int shader, unsigned int type)
{
    std::map<GLuint, std::string> map{
        {GL_PROGRAM,"GL_PROGRAM"},
        {GL_VERTEX_SHADER,"VertexShader"},
        {GL_FRAGMENT_SHADER,"FragmentShader"},
        {GL_GEOMETRY_SHADER,"GeometryShader"},
        {GL_TESS_CONTROL_SHADER,"TessControlShader"},
        {GL_TESS_EVALUATION_SHADER,"TessEvaluationShader"},
        {GL_COMPUTE_SHADER,"ComputeShader"}
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

unsigned int XBaseShader::compileShader(ShaderType type , const std::string& shaderCode)
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
	case ShaderType::COMPUTE:
		glType = GL_COMPUTE_SHADER;
		break;
    }
    

    GLuint shader = glCreateShader(glType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    checkCompileErrors(shader, glType);
    return shader;

}

XBaseShader XBaseShader::createFeedbackShader(unsigned int vs_id, const std::vector<std::string>& variables)
{
    XBaseShader shader;
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

XBaseShader XBaseShader::createComputeShader(unsigned int id)
{
    XBaseShader shader;
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, id);
    glLinkProgram(shader.ID);

    checkCompileErrors(shader.ID, GL_PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(id);

    return shader;
}
