#version 430 core
#define SMAA_DISABLE_DIAG_DETECTION
#define SMAA_PRESET_HIGH
#define SMAA_GLSL_4
#define SMAA_MAX_SEARCH_STEPS 32
#define SMAA_MAX_SEARCH_STEPS_DIAG 16
#define SMAA_CORNER_ROUNDING 25
#define SMAA_AREATEX_MAX_DISTANCE 16
#define SMAA_AREATEX_MAX_DISTANCE_DIAG 20
#define SMAA_AREATEX_PIXEL_SIZE (1.0 / vec2(160.0, 560.0))
#define SMAA_AREATEX_SUBTEX_SIZE (1.0 / 7.0)
#define SMAA_SEARCHTEX_SIZE vec2(66.0, 33.0)
#define SMAA_SEARCHTEX_PACKED_SIZE vec2(64.0, 16.0)
#define SMAA_CORNER_ROUNDING_NORM (float(SMAA_CORNER_ROUNDING) / 100.0)

uniform vec4 SMAA_RT_METRICS;
layout(binding = 3) uniform sampler2D edgesTex;
layout(binding = 4) uniform sampler2D areaTex;
layout(binding = 5) uniform sampler2D searchTex;
uniform vec4 subsampleIndices = vec4(0.0);

#define mad(a, b, c) fma(a, b, c)
#define saturate(a) clamp(a, 0.0, 1.0)
#define round(x) floor((x) + 0.5)

// 官方原版函数
float SMAASearchLength(sampler2D searchTex, vec2 e, float offset) {
    vec2 scale = SMAA_SEARCHTEX_SIZE * vec2(0.5, -1.0);
    vec2 bias = SMAA_SEARCHTEX_SIZE * vec2(offset, 1.0);
    scale += vec2(-1.0, 1.0);
    bias += vec2(0.5, -0.5);
    scale /= SMAA_SEARCHTEX_PACKED_SIZE;
    bias /= SMAA_SEARCHTEX_PACKED_SIZE;
    return textureLod(searchTex, mad(scale, e, bias), 0.0).r;
}

// -------------------------- 【已修复】搜索函数（先采样初始值，再先移动后采样） --------------------------
float SMAASearchXLeft(sampler2D edgesTex, sampler2D searchTex, vec2 texcoord, float end) {
    vec2 e = vec2(0.0, 1.0);
    //e = textureLod(edgesTex, texcoord, 0.0).rg; // 新增：先采样初始位置
    while (texcoord.x > end && e.g > 0.8281 && e.r == 0.0) {
        texcoord = mad(-vec2(2.0, 0.0), SMAA_RT_METRICS.xy, texcoord); // 先移动
        e = textureLod(edgesTex, texcoord, 0.0).rg; // 再采样
    }
    float offset = mad(-(255.0 / 127.0), SMAASearchLength(searchTex, e, 0.0), 3.25);
    return mad(SMAA_RT_METRICS.x, offset, texcoord.x);
}

float SMAASearchXRight(sampler2D edgesTex, sampler2D searchTex, vec2 texcoord, float end) {
    vec2 e = vec2(0.0, 1.0);
    //e = textureLod(edgesTex, texcoord, 0.0).rg; // 新增：先采样初始位置
    while (texcoord.x < end && e.g > 0.8281 && e.r == 0.0) {
        texcoord = mad(vec2(2.0, 0.0), SMAA_RT_METRICS.xy, texcoord); // 先移动
        e = textureLod(edgesTex, texcoord, 0.0).rg; // 再采样
    }
    float offset = mad(-(255.0 / 127.0), SMAASearchLength(searchTex, e, 0.5), 3.25);
    return mad(-SMAA_RT_METRICS.x, offset, texcoord.x);
}

float SMAASearchYUp(sampler2D edgesTex, sampler2D searchTex, vec2 texcoord, float end) {
    vec2 e = vec2(1.0, 0.0);
    //e = textureLod(edgesTex, texcoord, 0.0).rg; // 新增：先采样初始位置
    while (texcoord.y > end && e.r > 0.8281 && e.g == 0.0) {
        texcoord = mad(-vec2(0.0, 2.0), SMAA_RT_METRICS.xy, texcoord); // 先移动
        e = textureLod(edgesTex, texcoord, 0.0).rg; // 再采样
    }
    float offset = mad(-(255.0 / 127.0), SMAASearchLength(searchTex, e.gr, 0.0), 3.25);
    return mad(SMAA_RT_METRICS.y, offset, texcoord.y);
}

float SMAASearchYDown(sampler2D edgesTex, sampler2D searchTex, vec2 texcoord, float end) {
    vec2 e = vec2(1.0, 0.0);
    //e = textureLod(edgesTex, texcoord, 0.0).rg; // 新增：先采样初始位置
    while (texcoord.y < end && e.r > 0.8281 && e.g == 0.0) {
        texcoord = mad(vec2(0.0, 2.0), SMAA_RT_METRICS.xy, texcoord); // 先移动
        e = textureLod(edgesTex, texcoord, 0.0).rg; // 再采样
    }
    float offset = mad(-(255.0 / 127.0), SMAASearchLength(searchTex, e.gr, 0.5), 3.25);
    return mad(-SMAA_RT_METRICS.y, offset, texcoord.y);
}
// -----------------------------------------------------------------------------------

vec2 SMAAArea(sampler2D areaTex, vec2 dist, float e1, float e2, float offset) {
    vec2 texcoord = mad(vec2(SMAA_AREATEX_MAX_DISTANCE), round(4.0 * vec2(e1, e2)), dist);
    texcoord = mad(SMAA_AREATEX_PIXEL_SIZE, texcoord, 0.5 * SMAA_AREATEX_PIXEL_SIZE);
    texcoord.y = mad(SMAA_AREATEX_SUBTEX_SIZE, offset, texcoord.y);
    return textureLod(areaTex, texcoord, 0.0).rg;
}

vec2 SMAADecodeDiagBilinearAccess(vec2 e) {
    e.r = e.r * abs(5.0 * e.r - 5.0 * 0.75);
    return round(e);
}

vec4 SMAADecodeDiagBilinearAccess(vec4 e) {
    e.rb = e.rb * abs(5.0 * e.rb - 5.0 * 0.75);
    return round(e);
}

vec2 SMAASearchDiag1(sampler2D edgesTex, vec2 texcoord, vec2 dir, out vec2 e) {
    vec4 coord = vec4(texcoord, -1.0, 1.0);
    vec3 t = vec3(SMAA_RT_METRICS.xy, 1.0);
    while (coord.z < float(SMAA_MAX_SEARCH_STEPS_DIAG - 1) && coord.w > 0.9) {
        coord.xyz = mad(t, vec3(dir, 1.0), coord.xyz);
        e = textureLod(edgesTex, coord.xy, 0.0).rg;
        coord.w = dot(e, vec2(0.5, 0.5));
    }
    return coord.zw;
}

vec2 SMAASearchDiag2(sampler2D edgesTex, vec2 texcoord, vec2 dir, out vec2 e) {
    vec4 coord = vec4(texcoord, -1.0, 1.0);
    coord.x += 0.25 * SMAA_RT_METRICS.x;
    vec3 t = vec3(SMAA_RT_METRICS.xy, 1.0);
    while (coord.z < float(SMAA_MAX_SEARCH_STEPS_DIAG - 1) && coord.w > 0.9) {
        coord.xyz = mad(t, vec3(dir, 1.0), coord.xyz);
        e = textureLod(edgesTex, coord.xy, 0.0).rg;
        e = SMAADecodeDiagBilinearAccess(e);
        coord.w = dot(e, vec2(0.5, 0.5));
    }
    return coord.zw;
}

vec2 SMAAAreaDiag(sampler2D areaTex, vec2 dist, vec2 e, float offset) {
    vec2 texcoord = mad(vec2(SMAA_AREATEX_MAX_DISTANCE_DIAG), e, dist);
    texcoord = mad(SMAA_AREATEX_PIXEL_SIZE, texcoord, 0.5 * SMAA_AREATEX_PIXEL_SIZE);
    texcoord.x += 0.5;
    texcoord.y = mad(SMAA_AREATEX_SUBTEX_SIZE, offset, texcoord.y);
    return textureLod(areaTex, texcoord, 0.0).rg;
}

vec2 SMAACalculateDiagWeights(sampler2D edgesTex, sampler2D areaTex, vec2 texcoord, vec2 e, vec4 subsampleIndices) {
    vec2 weights = vec2(0.0);
    vec4 d;
    vec2 end;

    if (e.r > 0.0) {
        d.xz = SMAASearchDiag1(edgesTex, texcoord, vec2(-1.0,  1.0), end);
        d.x += float(end.y > 0.9);
    } else
        d.xz = vec2(0.0);
    d.yw = SMAASearchDiag1(edgesTex, texcoord, vec2( 1.0, -1.0), end);

    if (d.x + d.y > 2.0) {
        vec4 coords = mad(vec4(-d.x + 0.25, d.x, d.y, -d.y - 0.25), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
        vec4 c;
        c.xy = textureLodOffset(edgesTex, coords.xy, 0.0, ivec2(-1,  0)).rg;
        c.zw = textureLodOffset(edgesTex, coords.zw, 0.0, ivec2( 1,  0)).rg;
        c.yxwz = SMAADecodeDiagBilinearAccess(c.xyzw);
        vec2 cc = mad(vec2(2.0), c.xz, c.yw);
        weights += SMAAAreaDiag(areaTex, d.xy, cc, subsampleIndices.z);
    }

    d.xz = SMAASearchDiag2(edgesTex, texcoord, vec2(-1.0, -1.0), end);
    if (textureLodOffset(edgesTex, texcoord, 0.0, ivec2(1, 0)).r > 0.0) {
        d.yw = SMAASearchDiag2(edgesTex, texcoord, vec2( 1.0,  1.0), end);
        d.y += float(end.y > 0.9);
    } else
        d.yw = vec2(0.0);

    if (d.x + d.y > 2.0) {
        vec4 coords = mad(vec4(-d.x, -d.x, d.y, d.y), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
        vec4 c;
        c.x  = textureLodOffset(edgesTex, coords.xy, 0.0, ivec2(-1,  0)).g;
        c.y  = textureLodOffset(edgesTex, coords.xy, 0.0, ivec2( 0, -1)).r;
        c.zw = textureLodOffset(edgesTex, coords.zw, 0.0, ivec2( 1,  0)).gr;
        vec2 cc = mad(vec2(2.0), c.xz, c.yw);
        weights += SMAAAreaDiag(areaTex, d.xy, cc, subsampleIndices.w).gr;
    }
    return weights;
}

void SMAADetectHorizontalCornerPattern(sampler2D edgesTex, inout vec2 weights, vec4 texcoord, vec2 d) {
    vec2 leftRight = step(d.xy, d.yx);
    vec2 rounding = (1.0 - SMAA_CORNER_ROUNDING_NORM) * leftRight;
    rounding /= leftRight.x + leftRight.y;
    vec2 factor = vec2(1.0);
    factor.x -= rounding.x * textureLodOffset(edgesTex, texcoord.xy, 0.0, ivec2(0, 1)).r;
    factor.x -= rounding.y * textureLodOffset(edgesTex, texcoord.zw, 0.0, ivec2(1, 1)).r;
    factor.y -= rounding.x * textureLodOffset(edgesTex, texcoord.xy, 0.0, ivec2(0, -2)).r;
    factor.y -= rounding.y * textureLodOffset(edgesTex, texcoord.zw, 0.0, ivec2(1, -2)).r;
    weights *= saturate(factor);
}

void SMAADetectVerticalCornerPattern(sampler2D edgesTex, inout vec2 weights, vec4 texcoord, vec2 d) {
    vec2 leftRight = step(d.xy, d.yx);
    vec2 rounding = (1.0 - SMAA_CORNER_ROUNDING_NORM) * leftRight;
    rounding /= leftRight.x + leftRight.y;
    vec2 factor = vec2(1.0);
    factor.x -= rounding.x * textureLodOffset(edgesTex, texcoord.xy, 0.0, ivec2(1, 0)).g;
    factor.x -= rounding.y * textureLodOffset(edgesTex, texcoord.zw, 0.0, ivec2(1, 1)).g;
    factor.y -= rounding.x * textureLodOffset(edgesTex, texcoord.xy, 0.0, ivec2(-2, 0)).g;
    factor.y -= rounding.y * textureLodOffset(edgesTex, texcoord.zw, 0.0, ivec2(-2, 1)).g;
    weights *= saturate(factor);
}

out vec4 blendOut;

void main() {
    vec2 texcoord = gl_FragCoord.xy * SMAA_RT_METRICS.xy;
    vec2 pixcoord = texcoord * SMAA_RT_METRICS.zw;

    vec4 offset[3];
    offset[0] = fma(SMAA_RT_METRICS.xyxy, vec4(-0.25, -0.125,  1.25, -0.125), texcoord.xyxy);
    offset[1] = fma(SMAA_RT_METRICS.xyxy, vec4(-0.125, -0.25, -0.125,  1.25), texcoord.xyxy);
    offset[2] = fma(SMAA_RT_METRICS.xxyy, vec4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS), vec4(offset[0].xz, offset[1].yw));

    vec2 e = texture(edgesTex, texcoord).rg;
    vec4 weights = vec4(0.0);

    if (e.g > 0.0) {
        // ====================== 新增宏判断：禁用对角线 ======================
        #ifdef SMAA_DISABLE_DIAG_DETECTION
            // 宏开启：直接跳过所有对角线逻辑，只计算水平边缘
            vec2 d;
            vec3 coords;
            coords.x = SMAASearchXLeft(edgesTex, searchTex, offset[0].xy, offset[2].x);
            coords.y = offset[1].y;
            d.x = coords.x;
            float e1 = textureLod(edgesTex, coords.xy, 0.0).r;
            coords.z = SMAASearchXRight(edgesTex, searchTex, offset[0].zw, offset[2].y);
            d.y = coords.z;
            d = abs(round(fma(SMAA_RT_METRICS.zz, d, -pixcoord.xx)));
            vec2 sqrt_d = sqrt(d);
            float e2 = textureLodOffset(edgesTex, coords.zy, 0.0, ivec2(1, 0)).r;
            weights.rg = SMAAArea(areaTex, sqrt_d, e1, e2, subsampleIndices.y);
            coords.y = texcoord.y;
            SMAADetectHorizontalCornerPattern(edgesTex, weights.rg, coords.xyzy, d);
        #else
            // 宏关闭：原版逻辑（对角线+水平）
            vec2 diagWeights = SMAACalculateDiagWeights(edgesTex, areaTex, texcoord, e, subsampleIndices);
            if (abs(diagWeights.r + diagWeights.g) < 1e-5){
                weights.rg = diagWeights;
                e.r = 0.0;
            } else {
                vec2 d;
                vec3 coords;
                coords.x = SMAASearchXLeft(edgesTex, searchTex, offset[0].xy, offset[2].x);
                coords.y = offset[1].y;
                d.x = coords.x;
                float e1 = textureLod(edgesTex, coords.xy, 0.0).r;
                coords.z = SMAASearchXRight(edgesTex, searchTex, offset[0].zw, offset[2].y);
                d.y = coords.z;
                d = abs(round(fma(SMAA_RT_METRICS.zz, d, -pixcoord.xx)));
                vec2 sqrt_d = sqrt(d);
                float e2 = textureLodOffset(edgesTex, coords.zy, 0.0, ivec2(1, 0)).r;
                weights.rg = SMAAArea(areaTex, sqrt_d, e1, e2, subsampleIndices.y);
                coords.y = texcoord.y;
                SMAADetectHorizontalCornerPattern(edgesTex, weights.rg, coords.xyzy, d);
            }
        #endif
        // ==================================================================
    }

    if (e.r > 0.0) {
        vec2 d;
        vec3 coords;
        coords.y = SMAASearchYUp(edgesTex, searchTex, offset[1].xy, offset[2].z);
        coords.x = offset[0].x;
        d.x = coords.y;
        float e1 = textureLod(edgesTex, coords.xy, 0.0).g;
        coords.z = SMAASearchYDown(edgesTex, searchTex, offset[1].zw, offset[2].w);
        d.y = coords.z;
        d = abs(round(fma(SMAA_RT_METRICS.ww, d, -pixcoord.yy)));
        vec2 sqrt_d = sqrt(d);
        float e2 = textureLodOffset(edgesTex, coords.xz, 0.0, ivec2(0, 1)).g;
        weights.ba = SMAAArea(areaTex, sqrt_d, e1, e2, subsampleIndices.x);
        coords.x = texcoord.x;
        SMAADetectVerticalCornerPattern(edgesTex, weights.ba, coords.xyxz, d);
    }

    blendOut = weights;
}