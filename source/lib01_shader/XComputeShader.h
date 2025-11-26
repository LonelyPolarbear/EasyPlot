#ifndef XCOMPUTE_SHADER_HPP
#define XCOMPUTE_SHADER_HPP

#include<string>
#include <Eigen/Eigen>
#include <dataBase/dataobject.h>
#include "xshaderApi.h"
#include "XBaseShader.h"

//计算着色器
class lib01_shader_API XComputeShader:public XBaseShader {
 public:
    XComputeShader() = default;

    ~XComputeShader();

	XComputeShader(const std::string& codeOrPath, bool isPath = true);

    void loadComputeShader(const std::string& codeOrPath, bool isPath = true);

    void dispatchCompute(int x, int y, int z);
public:
};
#endif