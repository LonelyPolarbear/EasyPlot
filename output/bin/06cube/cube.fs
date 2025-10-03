//片段着色器
#version 430 core
 out vec4 FragColor;


  layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

layout(binding = 0) uniform samplerBuffer   tboSampler;

flat in vec4 elementColor;
#define USE_SSBO 1
#define USE_TBO 0
#define USE_GEOMETRY_PASS_COLOR 0

void main()
{	
#if USE_GEOMETRY_PASS_COLOR
	FragColor = elementColor;
#elif USE_TBO
	int Id = gl_PrimitiveID/2;
  vec4 color = texelFetch(tboSampler, Id);
	FragColor = color;
#elif USE_SSBO
	int Id = gl_PrimitiveID/2;
  vec4 color = MySSBO.data[Id];
	FragColor = color;
#endif
}