//片段着色器
#version 430 core
 out vec4 FragColor;

 layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																				//屏幕大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
};

const int COLOMODE_SINGLECOLOR = 1;
const int COLOMODE_VERTEXCOLOR = 2;
const int COLOMODE_FACECOLOR = 3;												//每个片元一个颜色
const int COLOMODE_TEXTURECOLOR = 4;										//每个片元一个颜色
const int COLOMODE_SELECT_TEST_COLOR = 5;							//每个片元一个颜色


const int POLYGONMODE_POINT = 1;
const int POLYGONMODE_LINE = 2;
const int POLYGONMODE_FILL = 3;

uniform int polygonMode;

uniform int colorMode;																		//颜色模式
uniform vec4 singleColor;																	//单色
uniform vec4 selectTestColor;															//选择测试颜色
uniform vec4 preSelectColor;															//预选颜色

uniform uint objectID;																		//当前对象ID

layout(binding = 1) uniform sampler2D depthSample;					//深度纹理，暂未用到
layout(binding = 2) uniform usampler2D objectIdSample;				//对象ID纹理(来自FBO)，用于预选


//顶点属性输入
in vec4 in_color;

void main()
{	
	if(colorMode == COLOMODE_SINGLECOLOR){
		FragColor = singleColor;
	}else{
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
}