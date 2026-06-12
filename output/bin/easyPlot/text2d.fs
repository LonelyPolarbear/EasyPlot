//片段着色器
#version 430 core
 out vec4 FragColor;

uniform vec4 singleColor;																								//单色
uniform float maxDerivate;
uniform float minDerivate;
layout(binding = 10) uniform sampler2DArray textTexture;												//字体纹理
in vec2 in_textureCoord;		
flat in float vs_TexLayer;//纹理坐标
void main()
{	

	vec4 color = texture(textTexture, vec3(in_textureCoord,vs_TexLayer));		
	float dis =color.r;
	dis -= 0.5;				//距离场变为-0.5 - 0.5

	float derivate = fwidth(dis);
	//float minDeriv = clamp(derivate, 0.05, 0.1);		//滤波宽度太宽会导致线条过于窄，丢失细节
	float minDeriv = clamp(derivate, minDerivate, maxDerivate);		//滤波宽度太宽会导致线条过于窄，丢失细节

	float alpha =smoothstep(-minDeriv, minDeriv, dis);
	float finalAlpha = alpha * singleColor.a;
	FragColor = vec4(singleColor.xyz, finalAlpha);
	//FragColor =vec4(1,0,0,1);
}