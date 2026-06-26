//顶点着色器
#version 430 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 100) out;

layout (std140, binding = 3) uniform ubo_gs
{
    uniform vec2 screenSizeTmp;													//屏幕大小
	uniform float sceneScale;															//当前场景的缩放比例
};

 layout(std430, binding = 1) buffer MySSBO_Block2  {
    int size; // 动态大小数组
    float data[]; // 动态大小数组
}MySSBO_Len;        //记录每一个图元的长度

uniform bool isComputeLineLentgh;
uniform bool connectSmoothEnable;												//连接处是否平滑
uniform bool isFixedLine;																//是否固定线宽,固定线宽时，lineWidth为固定值，不受缩放影响
uniform uint lineWidth;																	//属于每个图元
out vec2 texCoord;																			//纹理坐标，用于FSAA
flat out float SCalelineWidth;														//缩放后的线宽度，flat 传递给片元着色器
flat out float SCalelineWidthInGs;													//缩放后的线宽度，flat 传递给片元着色器
flat out float gs_lineLength;															//线段长度，用于计算线段的长度，用于计算线段的长度，用于计算线段的长度，用于计算线段

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

void emitLineSegment(vec4 pos0 ,vec4 pos1,vec4 pos2,vec4 pos3,vec2 screenSize_,float lineWidth_,float realLineWidth){
		vec4[4] quad;
		vec2 screen_p0=clip2screen(pos0,screenSize_);
		vec2 screen_p1=clip2screen(pos1,screenSize_);
		vec2 screen_p2=clip2screen(pos2,screenSize_);
		vec2 screen_p3=clip2screen(pos3,screenSize_);

		vec2 lineDir01 = normalize(screen_p1 - screen_p0); // 线的前向量的方向向量
		vec2 lineDir12 = normalize(screen_p2 - screen_p1); // 线的方向向量
		vec2 lineDir23 = normalize(screen_p3 - screen_p2); // 线的后向量的方向向量

		vec2 lineNormal = normalize(vec2(-lineDir12.y, lineDir12.x)); // 线的法向量

		//求01 和12的角平分线
		vec2 bisector012 =normalize( lineDir12-lineDir01);
		vec2 bisector123 =normalize( lineDir23-lineDir12);

		float lefteLenFactor =1./dot(lineNormal,bisector012);
		vec2 leftDir = bisector012;

		float rightLenFactor =1./dot(lineNormal,bisector123);
		vec2 rightDir = bisector123;

		if(length(screen_p1 - screen_p0)<=1){
			//说明是同一个点
			leftDir =lineNormal;
			lefteLenFactor = 1;
		}
		if(length(screen_p3 - screen_p2)<=1){
			//说明是同一个点
			rightDir =lineNormal;
			rightLenFactor = 1;
		}
	
		// 调整线段起点和终点，为圆角留出空间
		//float adjustDist = 0.5*lineWidth;
		float adjustDist = 0;
		vec2 adjusted_p1 = screen_p1 + lineDir12 * adjustDist;
		vec2 adjusted_p2 = screen_p2 - lineDir12 * adjustDist;
		vec2 screen_p10;
		vec2 screen_p11;
		vec2 screen_p20;
		vec2 screen_p21;
		if(connectSmoothEnable){
			screen_p10 = adjusted_p1 + leftDir *lineWidth_*0.5*lefteLenFactor; 
			screen_p11 = adjusted_p1 - leftDir * lineWidth_*0.5*lefteLenFactor; 

			screen_p20 = adjusted_p2 + rightDir *lineWidth_*0.5*rightLenFactor; 
			screen_p21 = adjusted_p2 - rightDir * lineWidth_*0.5*rightLenFactor;
		}else{
			screen_p10 = adjusted_p1 + lineNormal *lineWidth_*0.5; 
			screen_p11 = adjusted_p1 - lineNormal * lineWidth_*0.5; 

			screen_p20 = adjusted_p2 + lineNormal *lineWidth_*0.5; 
			screen_p21 = adjusted_p2 - lineNormal * lineWidth_*0.5;
		}
		
		//顺序 screen_p00 screen_p01 screen_p11 screen_p10

		vec4 clip0 = screen2clip(screen_p10,pos1);
		vec4 clip1 = screen2clip(screen_p11,pos1);

		vec4 clip2 = screen2clip(screen_p21,pos2);
		vec4 clip3 = screen2clip(screen_p20,pos2);

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
		texCoord = vec2(0,0);		//clip1
		SCalelineWidth = realLineWidth;
		SCalelineWidthInGs = lineWidth_;
		gs_lineLength = length(screen_p2 - screen_p1);
		MySSBO_Len.data[gl_PrimitiveIDIn] = gs_lineLength;

		if(isComputeLineLentgh){
			return;
		}
		EmitVertex();

		gl_Position = quad[2];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(1,0);		//clip2;
		EmitVertex();

		gl_Position = quad[0];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(0,1);		//clip0;
		EmitVertex();

		gl_Position = quad[3];
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(1,1);		//clip3;
		EmitVertex();
		SCalelineWidth =  realLineWidth;
		SCalelineWidthInGs = lineWidth_;
		gs_lineLength = length(screen_p2 - screen_p1);

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
	float sceneScale_ = sceneScale;

	//如果是固定线框，则不受缩放影响，sceneScale_固定为1
	if(isFixedLine){
		sceneScale_ = 1;
	}
	
	float minLineWidth =1;
	float lineWidth_ = max(lineWidth*sceneScale_,minLineWidth);					//是为了保证 lineWidth_ >= 2，避免出现线条粗细不一致的情况
	float realLineWidth = max( lineWidth,minLineWidth) * sceneScale_;			//真实线宽，用于FSAA,当线持续缩放时，线宽不变，但是会调整透明度，所以需要记录真实线宽
	emitLineSegment(gl_in[0].gl_Position,
									gl_in[1].gl_Position,
									gl_in[2].gl_Position,
									gl_in[3].gl_Position,
									screenSizeTmp,lineWidth_,realLineWidth);
}