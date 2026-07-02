//片段着色器
#version 430 core
 out vec4 FragColor;

uniform vec4 fillColor;																	//单色
void main()
{	
	FragColor = fillColor;																//输出颜色
}