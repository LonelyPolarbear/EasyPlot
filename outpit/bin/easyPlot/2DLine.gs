//顶点着色器
#version 430 core
layout (lines) in;
layout (triangle_strip, max_vertices = 100) out;
//layout (lines_adjacency) in;
//layout (line_strip, max_vertices = 2) out; // 输出线段，而不是三角形带

layout (std140, binding = 3) uniform ubo_gs
{
    uniform vec2 screenSizeTmp;						//屏幕大小
	uniform float sceneScale;										//当前场景的缩放比例
};

uniform uint lineWidth; //属于每个图元

//裁剪空间到屏幕空间的转换,屏幕空间的原点在左下角
vec2 clip2screen(vec4 clip0,vec2 screen){
	vec2 ndc= clip0.xy /clip0.w;
	ndc.x= (ndc.x+1)*0.5*screen.x;
	ndc.y= (ndc.y+1)*0.5*screen.y;
	return ndc;
}

//屏幕空间到Ndc空间的转换,屏幕空间的原点在左下角
vec2 screen2Ndc(vec2 screenPos,vec2 screen){
	vec2 ndc= vec2( 2*screenPos.x/screen.x-1,2*screenPos.y/screen.y-1);
	return ndc;
}

// 从屏幕坐标转换回裁剪坐标
vec4 screen2clip(vec2 screenPos, vec4 originalClip) {
    vec2 ndc = screen2Ndc(screenPos, screenSizeTmp);
    return vec4(ndc.x * originalClip.w, ndc.y * originalClip.w, originalClip.z, originalClip.w);
}

void emitLineSegment(vec4 pos0,vec4 pos1,vec2 screenSize_,float lineWidth_){
		vec4[4] quad;
		vec2 screen_p0=clip2screen(pos0,screenSize_);
		vec2 screen_p1=clip2screen(pos1,screenSize_);

		vec2 lineDir = normalize(screen_p1 - screen_p0); // 线的方向向量

		vec2 lineNormal = vec2(-lineDir.y, lineDir.x); // 线的法向量

		// 调整线段起点和终点，为圆角留出空间
		//float adjustDist = 0.5*lineWidth;
		float adjustDist = 0;
		vec2 adjusted_p0 = screen_p0 + lineDir * adjustDist;
		vec2 adjusted_p1 = screen_p1 - lineDir * adjustDist;

		vec2 screen_p00 = adjusted_p0 + lineNormal *lineWidth_*0.5; 
		vec2 screen_p01 = adjusted_p0 - lineNormal * lineWidth_*0.5; 

		vec2 screen_p10 = adjusted_p1 + lineNormal *lineWidth_*0.5; 
		vec2 screen_p11 = adjusted_p1 - lineNormal * lineWidth_*0.5;

		//顺序 screen_p00 screen_p01 screen_p11 screen_p10

		vec4 clip0 = screen2clip(screen_p00,pos0);
		vec4 clip1 = screen2clip(screen_p01,pos0);
		vec4 clip2 = screen2clip(screen_p11,pos1);
		vec4 clip3 = screen2clip(screen_p10,pos1);

		/*
		clip0--------------clip3
		|					         |
		|					         |
		clip1--------------clip2
		*/

		quad[0]=clip0;
		quad[1]=clip1;
		quad[2]=clip2;
		quad[3]=clip3;

		gl_Position = quad[1];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		EmitVertex();

		gl_Position = quad[2];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		EmitVertex();

		gl_Position = quad[0];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		EmitVertex();

		gl_Position = quad[3];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		EmitVertex();

		EndPrimitive();
}

#if 0
void emitVertexJoin(vec4 centerPos, vec4 neighbor1, vec4 neighbor2,float lineWidth) {
    vec2 center = clip2screen(centerPos, screenSize);
    vec2 dir1 = normalize(clip2screen(neighbor1, screenSize) - center);
    vec2 dir2 = normalize(clip2screen(neighbor2, screenSize) - center);
    
    // 计算两个方向之间的角度
    float angle1 = atan(dir1.y, dir1.x);
    float angle2 = atan(dir2.y, dir2.x);
    
    // 确保角度顺序正确
    if (angle2 < angle1) angle2 += 2 * 3.14159;
    
    // 生成圆角的采样点
    int segments = 8; // 圆角的细分段数，越多越平滑
    float step = (angle2 - angle1) / segments;
    
    // 输出圆角三角形带
    gl_Position = screen2clip(center, centerPos);
    EmitVertex();
    
    for (int i = 0; i <= segments; i++) {
        float angle = angle1 + step * i;
        vec2 offset = vec2(cos(angle), sin(angle)) * lineWidth * 0.5;
        gl_Position = screen2clip(center + offset, centerPos);
        EmitVertex();
    }
    
    EndPrimitive();
}

#endif

void main()
{	
#if 0
	 // 只输出中心线段（忽略邻接顶点）
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
#endif

	emitLineSegment(gl_in[0].gl_Position,gl_in[1].gl_Position,screenSizeTmp,lineWidth*sceneScale);
}