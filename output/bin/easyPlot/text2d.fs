//片段着色器
#version 430 core
 out vec4 FragColor;

uniform vec4 singleColor;																									//单色
layout(binding = 6) uniform sampler2DArray textTexture;												//字体纹理
in vec2 vs_TexCoord;		
flat in float vs_TexLayer;//纹理坐标
void main()
{	
#if 0
	vec4 color = texture(textTexture, vec3(vs_TexCoord,vs_TexLayer));							//获取字体颜色
	vec4 color2 = vec4(1,0,0,1);																							//输出颜色
	FragColor =mix(color,color2,0.5);																					//输出颜色
#else
	vec4 color = texture(textTexture, vec3(vs_TexCoord,vs_TexLayer));		
	float dis =color.r;
	dis -= 0.5;				//距离场变为-0.5 - 0.5

	float derivate = fwidth(dis);
	float alpha =smoothstep(-derivate, derivate, dis);
	
	FragColor = vec4(singleColor.xyz, alpha);
	//FragColor = vec4(1,0,0,1);
#endif
}