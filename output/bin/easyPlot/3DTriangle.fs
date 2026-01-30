//片段着色器
#version 430 core
 out vec4 FragColor;

 layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

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

uniform int colorMode;																		//颜色模式
uniform vec4 singleColor;																	//单色
uniform vec4 selectTestColor;															//选择测试颜色
uniform vec4 preSelectColor;															//预选颜色

uniform uint objectID;																		//当前对象ID

layout(binding = 1) uniform sampler2D depthSample;					//深度纹理，暂未用到
layout(binding = 2) uniform usampler2D objectIdSample;				//对象ID纹理(来自FBO)，用于预选


//顶点属性输入
in vec4 in_color;

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
		return vec4(1,1,1,1);
		vec3 hsv = rgb2hsv(inputColor.xyz);
        hsv.z *= 0.5;								// 明度减半（可调整系数，如 0.6、0.7）
        hsv.z = max(hsv.z, 0.0);				// 防止负值
        vec3 color = hsv2rgb(hsv);
		return vec4(color,1);
	}
}
/********************************************************************************************************************/

void main()
{	
	vec4 fragcolor_before = vec4(0,0,0,1);
	#if 1
	if(polygonMode == POLYGONMODE_FACE){
			uint objectId = texelFetch( objectIdSample,ivec2(setRealTimeMousePos.x,setRealTimeMousePos.y),0).r;
			uint primitiveId = texelFetch( objectIdSample,ivec2(setRealTimeMousePos.x,setRealTimeMousePos.y),0).g;
			if(objectId == objectID){
				if(gl_PrimitiveID == primitiveId){
					//当前选中的图元
					fragcolor_before = preSelectColor;
				}else{
					//其他图元
					if(colorMode == COLOMODE_FACECOLOR){
						fragcolor_before = MySSBO.data[gl_PrimitiveID];
					}else if(colorMode == COLOMODE_VERTEXCOLOR){
						fragcolor_before = in_color;
					}else if(colorMode == COLOMODE_SINGLECOLOR){
						fragcolor_before = singleColor;
					}else if(colorMode == COLOMODE_SELECT_TEST_COLOR){
						fragcolor_before = selectTestColor;
					}else{
						fragcolor_before = vec4(1.0, 0.0, 1.0, 1.0);
					}
				}
			}else{
				if(colorMode == COLOMODE_FACECOLOR){
					fragcolor_before = MySSBO.data[gl_PrimitiveID];
				}else if(colorMode == COLOMODE_VERTEXCOLOR){
					fragcolor_before = in_color;
				}else if(colorMode == COLOMODE_SINGLECOLOR){
					fragcolor_before = singleColor;
				}else if(colorMode == COLOMODE_SELECT_TEST_COLOR){
					fragcolor_before = selectTestColor;
				}else{
					fragcolor_before = vec4(1.0, 0.0, 1.0, 1.0);
				}
			}
		}
	
	if(polygonMode == POLYGONMODE_LINE){
		fragcolor_before = singleColor;
	}

	if(polygonMode == POLYGONMODE_POINT){
		fragcolor_before = singleColor;
	}

	FragColor = setColorByOrien(fragcolor_before);
	#endif

	#if 0
		vec2 coord = vec2( gl_FragCoord.x / float(screenSize.x), gl_FragCoord.y / float( screenSize.y));
		float depthValue = texture(depthSample, coord.xy).r;
		FragColor = vec4(vec3(depthValue), 1.0);
	#endif
}