//片段着色器
#version 430 core
layout (location =0) out vec4 FragColor;
layout (location =1) out uvec4 out_objectId;

uniform int FaceColorBufferSize;
uniform int PrimitiveVertexNum;
uniform int PrimitiveLineNum;
uniform int PrimitiveFaceNum;
 layout(std430, binding = 0) buffer MySSBO_Block1  {
    vec4 data[]; // 动态大小数组
}	FaceColorBuffer;

 layout(std430, binding = 1) buffer MySSBO_Block2  {
    uint data[]; 
}	PrimitiveVertexStateBuffer;

layout(std430, binding = 2) buffer MySSBO_Block3  {
    uint data[]; 
}	PrimitiveLineStateBuffer;

layout(std430, binding = 3) buffer MySSBO_Block4  {
    uint data[]; 
}	PrimitiveFaceStateBuffer;

//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小,是当前render的窗口大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};


const int COLOMODE_SINGLECOLOR = 1;
const int COLOMODE_VERTEXCOLOR = 2;
const int COLOMODE_FACECOLOR = 3;												//每个片元一个颜色
const int COLOMODE_TEXTURECOLOR = 4;										//每个片元一个颜色
const int COLOMODE_SELECT_TEST_COLOR = 5;							//每个片元一个颜色


const int POLYGONMODE_POINT = 1;
const int POLYGONMODE_LINE = 2;
const int POLYGONMODE_FACE = 4;

uniform int polygonMode;

uniform int colorMode;																			//颜色模式
uniform vec4 singleColor;																		//单色
uniform vec4 selectedColor;																	//选择测试颜色
uniform vec4 preSelectColor;																//预选颜色
uniform uint PickMode;
uniform uint objectID;																			//当前对象ID

const uint STATE_NORMAL =1;
const uint STATE_PRESELECT =2;
const uint STATE_SELECT =4;

//layout(binding = 1) uniform sampler2D depthSample;						//深度纹理，暂未用到
layout(binding = 2) uniform usampler2D objectIdSample;					//对象ID纹理(来自FBO)，用于预选
layout(binding = 3) uniform usampler2D objectDiffuseTex;				//漫反射贴图
layout(binding = 4) uniform usampler2D objectNormalTex;				//法线贴图
layout(binding = 5) uniform usampler2D objectSpecularTex;				//镜面光贴图

//顶点属性输入
in vec4 in_color;
in vec2 in_textureCoord;
flat in int in_instanceID;
/********************************************************************************************************************/
/*一些函数*/
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV → RGB 转换
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

//正面 背面设置不同的颜色
vec4 setColorByOrien(vec4 inputColor){
	if(gl_FrontFacing){
		return inputColor;
	}else{
		return vec4(1,0,0,1);
		vec3 hsv = rgb2hsv(inputColor.xyz);
        hsv.z *= 0.5;								// 明度减半（可调整系数，如 0.6、0.7）
        hsv.z = max(hsv.z, 0.0);				// 防止负值
        vec3 color = hsv2rgb(hsv);
		return vec4(color,1);
	}
}
/********************************************************************************************************************/

vec4 getColorByState(uint state,vec4 bakgroundColor){
	if(state ==STATE_NORMAL ){
		return bakgroundColor;
	}
	//仅预选
	if(state == STATE_NORMAL+STATE_PRESELECT){
		return mix(bakgroundColor,preSelectColor,0.4);		//混合
	}

	if((state & STATE_SELECT) !=0){
		//判断是否包含预选
		if((state & STATE_PRESELECT) !=0){
			//表示选中的同时 预选
			vec4 c = mix(bakgroundColor,selectedColor,0.8);
			return mix(c,preSelectColor,0.4);		//混合
		}else{
			return mix(bakgroundColor,selectedColor,0.8);
		}
	}
	return vec4(1.0);
}

void main()
{	
	vec4 fragcolor_before = vec4(0,0,0,1);
	if(polygonMode == POLYGONMODE_FACE){
			//uint objectId = texelFetch( objectIdSample,ivec2(setRealTimeMousePos.x,setRealTimeMousePos.y),0).g;
			//uint primitiveId = texelFetch( objectIdSample,ivec2(setRealTimeMousePos.x,setRealTimeMousePos.y),0).b;
			if(colorMode == COLOMODE_FACECOLOR){
				fragcolor_before = FaceColorBuffer.data[gl_PrimitiveID];
				uint state = PrimitiveFaceStateBuffer.data[gl_PrimitiveID];
				//判断状态
				fragcolor_before = getColorByState(state,fragcolor_before);
			}else if(colorMode == COLOMODE_VERTEXCOLOR){
				fragcolor_before = in_color;
			}else if(colorMode == COLOMODE_SINGLECOLOR){
				fragcolor_before = singleColor;
			}else if(colorMode == COLOMODE_TEXTURECOLOR){
				fragcolor_before = texture(objectDiffuseTex,in_textureCoord);
			}else{
				fragcolor_before = vec4(1.0, 0.0, 1.0, 1.0);
			}
	}
	if(polygonMode == POLYGONMODE_LINE){
		fragcolor_before = singleColor;
	}

	if(polygonMode == POLYGONMODE_POINT){
		fragcolor_before = singleColor;
	}

	//FragColor = setColorByOrien(fragcolor_before);
	FragColor = fragcolor_before;

	//实例化ID 模型ID 图元ID //最低位 PickMode 其余备用
	uint extraData = 0u;
	extraData  = PickMode;

	//是否允许拾取
	out_objectId = uvec4(in_instanceID, objectID,gl_PrimitiveID,extraData);
}