#version 430 core


uniform vec4 SMAA_RT_METRICS;
uniform vec2 viewPortOrigin;
layout(binding = 3) uniform sampler2DMS  colorTex;   // 输入：原始场景颜色


// ----------------------------- 主函数 -----------------------------
out vec4 fragColor;
uniform bool u_enableSmaa;
void main() {
    int sampleNum = 8;
   vec2 texcoord = (gl_FragCoord.xy-viewPortOrigin) * SMAA_RT_METRICS.xy;
    ivec2 pixelCoord  = ivec2(gl_FragCoord.xy-viewPortOrigin);
    if(!u_enableSmaa){
         vec4 color = vec4(0);
        for (int i = 0; i < sampleNum; i++) {
            color += texelFetch(colorTex, pixelCoord, i);
        }
        color = color / float(sampleNum);
        fragColor = vec4(color.xyz,1);
    }else{
        fragColor = vec4(1,0,0,1);
    }
}