#version 430 core
#define SMAA_PREDICATION
#define SMAA_PRESET_HIGH
#define SMAA_GLSL_4
#define SMAA_THRESHOLD 0.01
#define SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR 2.0

uniform vec4 SMAA_RT_METRICS;
layout(binding = 3) uniform sampler2D colorTex;

out vec4 edgesOut;

void main() {
    vec2 texcoord = gl_FragCoord.xy * SMAA_RT_METRICS.xy;
    
    vec4 offset[3];
    offset[0] = fma(SMAA_RT_METRICS.xyxy, vec4(-1.0, 0.0, 0.0, -1.0), texcoord.xyxy);
    offset[1] = fma(SMAA_RT_METRICS.xyxy, vec4( 1.0, 0.0, 0.0,  1.0), texcoord.xyxy);
    offset[2] = fma(SMAA_RT_METRICS.xyxy, vec4(-2.0, 0.0, 0.0, -2.0), texcoord.xyxy);

    vec2 threshold = vec2(SMAA_THRESHOLD);

    vec3 weights = vec3(0.2126, 0.7152, 0.0722);
    float L = dot(texture(colorTex, texcoord).rgb, weights);
    float Lleft = dot(texture(colorTex, offset[0].xy).rgb, weights);
    float Ltop  = dot(texture(colorTex, offset[0].zw).rgb, weights);

    vec4 delta;
    delta.xy = abs(L - vec2(Lleft, Ltop));
    vec2 edges = step(threshold, delta.xy);

   // if (dot(edges, vec2(1.0)) == 0.0) {
   //     discard;
  //  }

    if (dot(edges, vec2(1.0)) == 0.0) {
    edgesOut = vec4(0.0, 0.0, 0.0, 1.0); // 非边缘像素明确输出黑色
    return;
}

    float Lright = dot(texture(colorTex, offset[1].xy).rgb, weights);
    float Lbottom = dot(texture(colorTex, offset[1].zw).rgb, weights);
    delta.zw = abs(L - vec2(Lright, Lbottom));

    vec2 maxDelta = max(delta.xy, delta.zw);

    float Lleftleft = dot(texture(colorTex, offset[2].xy).rgb, weights);
    float Ltoptop = dot(texture(colorTex, offset[2].zw).rgb, weights);
    delta.zw = abs(vec2(Lleft, Ltop) - vec2(Lleftleft, Ltoptop));

    maxDelta = max(maxDelta.xy, delta.zw);
    float finalDelta = max(maxDelta.x, maxDelta.y);

    edges.xy *= step(finalDelta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta.xy);

    // ====================== 【新增】SMAA_PREDICATION 子像素预测（官方原版）======================
    #ifdef SMAA_PREDICATION
        vec2 deltaPred;
        deltaPred.x = abs(L - 0.5 * (Lleft + Lright));
        deltaPred.y = abs(L - 0.5 * (Ltop + Lbottom));
        vec2 edgesPred = step(threshold * 0.5, deltaPred);
        edges.xy = max(edges.xy, edgesPred);
    #endif
    // ==========================================================================================

    edgesOut = vec4(edges, 0.0, 1.0);
}