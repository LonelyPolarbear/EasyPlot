#include <fstream>
#include <sstream>
#include <iostream>
#include <glew/glew.h>
#include "XComputeShader.h"

static std::string readFile(const std::string& path) {
    std::ifstream ShaderFile;
    ShaderFile.open(path);

    std::stringstream ShaderStream;
    // read file's buffer contents into streams
    ShaderStream << ShaderFile.rdbuf();

    ShaderFile.close();
    return ShaderStream.str();
}
XComputeShader::~XComputeShader()
{
    //glDeleteShader(ID);
}

XComputeShader::XComputeShader(const std::string& codeOrPath, bool isPath)
{
	loadComputeShader(codeOrPath,isPath);
}

void XComputeShader::loadComputeShader(const std::string& codeOrPath, bool isPath)
{
	std::string vertexCode;
	if (isPath) {
		vertexCode = readFile(codeOrPath);
	}
	else {
		vertexCode = codeOrPath;
	}

	unsigned int vertex, fragment;
	vertex = compileShader(ShaderType::COMPUTE, vertexCode);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glLinkProgram(ID);
	checkCompileErrors(ID, GL_PROGRAM);

	glDeleteShader(vertex);
}

void XComputeShader::dispatchCompute(int x, int y, int z)
{
	use();
	glDispatchCompute(x, y, z);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
