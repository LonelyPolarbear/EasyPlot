//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTextureCoord;

out vec4 in_color;
out vec2 in_textureCoord;


uniform mat4 ModelMat;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};
void main()
{	
	gl_Position = ProjectionMat*ViewMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}