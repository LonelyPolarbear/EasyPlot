//顶点着色器
#version 430 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 100) out;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
    mat4 Single_ViewMat;
    mat4 Single_ProjMat;
    mat4 NearplaneFrame;
    mat4 VirtualScreenFrame;
};

layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小,是当前render的窗口大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};

 layout(std430, binding = 5) buffer MySSBO_Block5  {
    int size;											// 动态大小数组
    float data[];									// 动态大小数组
}MySSBO_Len;									//记录每一个图元的长度

uniform bool u_isComputeLineLentgh;
uniform uint u_lineWidth;																	//属于每个图元
uniform vec3 u_planeNormal;														//直线所在平面的法线
uniform float u_offset;																	//为了处理直线重叠加的偏移，该值是剪切空间的值

const int CAMERA_MODE_3D_NORMAL=1;
const int CAMERA_MODE_3D_AXIS=2;
const int CAMERA_MODE_2D=3;
uniform int cameraMode;

in vec4 vWorldPos[];
flat out float gs_lineLength;															//线段长度，用于计算线段的长度
out vec2 texCoord;																			//纹理坐标，用于FSAA


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
    vec2 ndc = screen2Ndc(screenPos, screenSize);
    return vec4(ndc.x * originalClip.w, ndc.y * originalClip.w, originalClip.z, originalClip.w);
}


vec4 world2ndc(vec3 worldpos){
	 if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
           return  ProjectionMat*ViewMat*vec4(worldpos,1);
        }else  if(cameraMode ==CAMERA_MODE_3D_AXIS ){    
            return  Single_ProjMat*Single_ViewMat*vec4(worldpos,1);
        }else{  
            return ProjectionMat*NearplaneFrame*VirtualScreenFrame*vec4(worldpos,1);
        }
	return vec4(1,0,0,1);
}

void emitLineSegment(vec4 pos0 ,vec4 pos1,vec4 pos2,vec4 pos3,vec3 planeNormal,float lineWidth){
		vec3 lineDir01 = normalize(pos1.xyz - pos0.xyz);			// 线的前向量的方向向量
		vec3 lineDir12 = normalize(pos2.xyz - pos1.xyz);				// 线的方向向量
		vec3 lineDir23 = normalize(pos3.xyz - pos2.xyz);			// 线的后向量的方向向量

		//如果三点共线，则直接使用法向量作为偏移方向
		vec3 world_p10;
		vec3 world_p11;
		vec3 world_p20;
		vec3 world_p21;
		if(length(cross(lineDir12,lineDir01))<0.0001){
			vec3 lineNormal =normalize(cross(planeNormal,lineDir12));
			world_p10 = pos1.xyz + lineNormal *lineWidth*0.5; 
			world_p11 = pos1.xyz - lineNormal *lineWidth*0.5;
		}else{
			vec3 bisector012 =normalize( lineDir12-lineDir01);
			vec3 lineNormal =normalize(cross(planeNormal,lineDir12));
			float lefteLenFactor = 1./dot(lineNormal,bisector012);
			vec3 leftDir = bisector012;
			world_p10 = pos1.xyz + leftDir *lineWidth*0.5*lefteLenFactor; 
			world_p11 = pos1.xyz - leftDir *lineWidth*0.5*lefteLenFactor; 
		}

		if(length(cross(lineDir12,lineDir23))<0.0001){
			vec3 lineNormal =normalize(cross(planeNormal,lineDir12));
			world_p20 = pos2.xyz + lineNormal *lineWidth*0.5; 
			world_p21 = pos2.xyz - lineNormal *lineWidth*0.5;
		}else{
			vec3 bisector123 =normalize( lineDir23-lineDir12);
			vec3 lineNormal =normalize(cross(planeNormal,lineDir12));
			float rightLenFactor = 1./dot(lineNormal,bisector123);
			vec3 rightDir = bisector123;
			world_p20 = pos2.xyz + rightDir *lineWidth*0.5*rightLenFactor; 
			world_p21 = pos2.xyz - rightDir *lineWidth*0.5*rightLenFactor; 
		}
		
		vec4 ndc_p10 = world2ndc(world_p10);	ndc_p10.z +=u_offset;
		vec4 ndc_p11 = world2ndc(world_p11);	ndc_p11.z +=u_offset;
		vec4 ndc_p20 = world2ndc(world_p20);	ndc_p20.z +=u_offset;
		vec4 ndc_p21 = world2ndc(world_p21);	ndc_p21.z +=u_offset;

		gl_Position = ndc_p11;
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(0,0);
		EmitVertex();

		gl_Position = ndc_p21;
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(1,0);
		EmitVertex();

		gl_Position = ndc_p10;
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(0,1);
		EmitVertex();

		gl_Position = ndc_p20;
		gl_PrimitiveID =gl_PrimitiveIDIn;
		texCoord = vec2(1,1);
		EmitVertex();

		EndPrimitive();
}

void main()
{	
	//if(u_isComputeLineLentgh){
	//	gs_lineLength = length(vWorldPos[2] - vWorldPos[1]);
	//	MySSBO_Len.data[gl_PrimitiveIDIn] = gs_lineLength;
	//	return;
	//}

	emitLineSegment(
		vWorldPos[0],
		vWorldPos[1],
		vWorldPos[2],
		vWorldPos[3],
		u_planeNormal,
		u_lineWidth
	);
}