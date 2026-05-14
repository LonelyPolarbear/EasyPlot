#version 430 core


uniform vec4 SMAA_RT_METRICS;
layout(binding = 3) uniform sampler2DMS  colorTex;   // 输入：原始场景颜色


// ----------------------------- 主函数 -----------------------------
out vec4 fragColor;
uniform bool u_enableSmaa;
void main() {
    int sampleNum = 8;
   vec2 texcoord = gl_FragCoord.xy * SMAA_RT_METRICS.xy;
    ivec2 pixelCoord  = ivec2(gl_FragCoord.xy);
    if(!u_enableSmaa){
         vec4 color = vec4(0.0);
        for (int i = 0; i < sampleNum; i++) {
            color += texelFetch(colorTex, pixelCoord, i);
        }
        fragColor = color / float(sampleNum);
    }else{
        fragColor = vec4(0,0,0,1);
    }
}