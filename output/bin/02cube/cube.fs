//片段着色器
#version 430 core
 out vec4 FragColor;


  layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

layout(binding = 0) uniform samplerBuffer   tboSampler;


#define USE_SSBO 1
#define USE_TBO 0

void main()
{	
#if USE_TBO
	int Id = gl_PrimitiveID/2;
  vec4 color = texelFetch(tboSampler, Id);
	FragColor = color;
#endif

#if USE_SSBO
	int Id = gl_PrimitiveID/2;
  vec4 color = MySSBO.data[Id];
	FragColor = color;
#endif
}