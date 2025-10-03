//顶点着色器
#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
flat out vec4 elementColor;

layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

uniform bool isWireframe; // 是否为线框模式
uniform vec4 wireframeColor; // 线框颜色
void main()
{	
	for(int i=0; i<gl_in.length(); i++) {
           gl_Position = gl_in[i].gl_Position;
		   gl_PrimitiveID  =gl_PrimitiveIDIn; // 传递到片段着色器
		   if(isWireframe) { // 线框模式		  
			   elementColor = wireframeColor;
		   }else{
				elementColor = MySSBO.data[gl_PrimitiveIDIn/2]; // 传递颜色
		   }
           EmitVertex();
       }
       EndPrimitive();
}