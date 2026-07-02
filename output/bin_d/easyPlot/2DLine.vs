//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;


uniform mat4 ModelMat;
//uniform mat4 ViewMat;
//uniform mat4 ProjectionMat;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};
void main()
{	
	gl_Position = ProjectionMat*ViewMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}