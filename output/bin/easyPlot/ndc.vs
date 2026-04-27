//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec4 in_color;
void main()
{	
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	in_color = vec4(aColor,1);
}