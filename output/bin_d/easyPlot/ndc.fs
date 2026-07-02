//片段着色器
#version 430 core
 out vec4 FragColor;


//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小,是当前render的窗口大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};

layout(binding = 1) uniform sampler2D depthSample;						//深度纹理，暂未用到
layout(binding = 2) uniform usampler2D objectIdSample;					//对象ID纹理(来自FBO)，用于预选
layout(binding = 3) uniform usampler2D objectDiffuseTex;				//漫反射贴图
layout(binding = 4) uniform usampler2D objectNormalTex;				//法线贴图
layout(binding = 5) uniform usampler2D objectSpecularTex;				//镜面光贴图

//顶点属性输入
in vec4 in_color;
in vec2 in_textureCoord;

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
#define FXAA_REDUCE_MIN   (1.0 / 128.0)  // 最小减少因子，防止过度模糊
#define FXAA_REDUCE_MUL   (1.0 / 8.0)    // 减少采样步长的乘数因子
#define FXAA_SPAN_MAX     16.0            // 最大采样跨度，控制抗锯齿范围
#define FXAA_THRESHOLD    0.125          // 亮度对比度阈值，控制抗锯齿触发灵敏度

void main() {
    // 1. 计算单个像素的 UV 步长
    vec2 inverseResolution = 1.0 / screenSize;
    
    // 2. 采样当前像素及周围8个像素的亮度 (Luma)
    // 使用经典的 NTSC 亮度转换公式: Luma = 0.299R + 0.587G + 0.114B
    vec3 rgbNW = texture(objectDiffuseTex, in_textureCoord + vec2(-1.0, -1.0) * inverseResolution).xyz;
    vec3 rgbNE = texture(objectDiffuseTex, in_textureCoord + vec2( 1.0, -1.0) * inverseResolution).xyz;
    vec3 rgbSW = texture(objectDiffuseTex, in_textureCoord + vec2(-1.0,  1.0) * inverseResolution).xyz;
    vec3 rgbSE = texture(objectDiffuseTex, in_textureCoord + vec2( 1.0,  1.0) * inverseResolution).xyz;
    vec3 rgbM  = texture(objectDiffuseTex, in_textureCoord).xyz;
    
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    
    // 3. 寻找 3x3 区域内的最小和最大亮度
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    // 4. 边缘检测：如果亮度差异小于阈值，说明不是边缘，直接返回原色
    if (lumaMax - lumaMin <= lumaMax * FXAA_THRESHOLD) {
        FragColor = vec4(rgbM, 1.0);
        return;
    }
    
    // 5. 计算梯度方向（边缘的法线方向）
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // 6. 计算采样步长缩减因子
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    // 限制采样方向的最大跨度，并转换为实际的 UV 偏移量
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * inverseResolution;
    
    // 7. 沿着梯度方向进行两次采样并取平均 (Two-Tap)
    vec3 rgbA = 0.5 * (
        texture(objectDiffuseTex, in_textureCoord + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture(objectDiffuseTex, in_textureCoord + dir * (2.0 / 3.0 - 0.5)).xyz);
    
    // 8. 再进行更远距离的两次采样，做二次混合 (Four-Tap)
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(objectDiffuseTex, in_textureCoord + dir * -0.5).xyz +
        texture(objectDiffuseTex, in_textureCoord + dir * 0.5).xyz);
    
    // 9. 检查最终采样的亮度是否超出原 3x3 区域的亮度范围
    // 如果超出，说明采样到了另一侧的强边缘，回退到 Two-Tap 的结果以避免颜色溢出
    float lumaB = dot(rgbB, luma);
    if (lumaB < lumaMin || lumaB > lumaMax) {
        FragColor = vec4(rgbA, 1.0);
    } else {
        FragColor = vec4(rgbB, 1.0);
    }
}