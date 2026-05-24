//片段着色器
#version 430 core
layout (location =0) out vec4 FragColor;

//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小,是当前render的窗口大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};


layout(binding = 3) uniform usampler2DMS IdSample;	                                    //物体ID纹理信息
layout(binding = 4) uniform usampler2DMS ColorSample;	
layout(binding = 5) uniform sampler2D DepthSample;	

//顶点属性输入
in vec4 in_color;
in vec2 in_textureCoord;
flat in int in_instanceID;
/**************************************************************************************************************/
//如果coord处有一个样本点的id是0，说明就是边界
bool haObjectZero(ivec2 coordInput,ivec2 dir, int samples,int step)
{
    for(int i=1;i<=step;++i){
        ivec2 coord = coordInput+dir*i;
        if (coord.x < 0 || coord.y < 0 || coord.x >= int(screenSize.x) || coord.y >= int(screenSize.y))
            return false;
    
        for (int j = 0; j < samples; ++j) {
            uint stencilVal = uint(texelFetch(IdSample, coord, j).g);
            if (stencilVal == 0u)
                return true;
        }
    }
    return false;
}

#if 1
void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    const int samples = 8;

    // ---- 深度测试（软遮挡） ----
    float depth = texture(DepthSample, coord).r;
    float depthDiff = gl_FragCoord.z - depth;
    float occlusion = 1.0;
    if (depthDiff > 0.0) {
        occlusion = 1.0 - smoothstep(0.0, 0.0002, depthDiff);
        if (occlusion < 0.01) discard;
    }

    // ---- 当前覆盖率 ----
    int count = 0;
    for (int i = 0; i < samples; ++i) {
        if (texelFetch(IdSample, coord, i).g != 0u) count++;
    }
    if (count == 0) discard;
    float coverage = float(count) / float(samples);

    // ---- 颜色 ----
    vec3 color = vec3(0.0);
    for (int i = 0; i < samples; ++i)
        color += texelFetch(ColorSample, coord, i).rgb;
    color /= float(samples);

    // ---- 边缘强度 ----
    float edgeStrength = 0.0;
    if (coverage < 1.0) {
        edgeStrength = 1.0 - coverage;
    } else {
        int radius = 2;
        float weightSum = 0.0;
        float weightedBorder = 0.0;
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                if (dx == 0 && dy == 0) continue;
                ivec2 nCoord = coord + ivec2(dx, dy);
                if (nCoord.x < 0 || nCoord.y < 0 || 
                    nCoord.x >= int(screenSize.x) || nCoord.y >= int(screenSize.y)) continue;

                int nCount = 0;
                for (int i = 0; i < samples; ++i)
                    if (texelFetch(IdSample, nCoord, i).g != 0u) nCount++;
                float nCoverage = float(nCount) / float(samples);
                float border = 1.0 - nCoverage;

                if (border > 0.001) {
                    float dist = length(vec2(dx, dy));
                    float w = 1.0 / (dist + 1.0);
                    weightedBorder += border * w;
                    weightSum += w;
                }
            }
        }
        if (weightSum > 0.0) {
            float avgBorder = weightedBorder / weightSum;
            edgeStrength = smoothstep(0.0, 0.6, avgBorder);
        }
    }

    // 全局柔化 + 遮挡
    edgeStrength = smoothstep(0.0, 1.0, edgeStrength);
    edgeStrength *= occlusion;

    // ---- 输出 ----
    if (edgeStrength > 0.01) {
        vec3 edgeColor = vec3(1.0, 1.0, 0.0);
        vec3 finalColor = mix(color, edgeColor, edgeStrength);
        FragColor = vec4(finalColor, 1.0);
    } else {
        discard;
    }
}
#endif