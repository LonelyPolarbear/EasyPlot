#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aInstanceMatC0l1;
layout (location = 2) in vec4 aInstanceMatC0l2;
layout (location = 3) in vec4 aInstanceMatC0l3;
layout (location = 4) in vec4 aInstanceMatC0l4;

layout (location = 5) in vec3 aTextCoordPoint1;   //纹理坐标
layout (location = 6) in vec3 aTextCoordPoint2;   //纹理坐标
layout (location = 7) in vec3 aTextCoordPoint3;   //纹理坐标
layout (location = 8) in vec3 aTextCoordPoint4;   //纹理坐标

uniform bool isInstance;
uniform mat4 ModelMat;
out vec2 vs_TexCoord;
flat out float vs_TexLayer;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};

layout (std140, binding = 4) uniform Matrices2d
{
    mat4 virtualWorldFrame;
	mat4 sceneFrameInVirtualWorld;																			                                                //鼠标实时位置，用于预选
};


void main()
{	
	if(gl_VertexID ==0){
		 vs_TexCoord = aTextCoordPoint1.xy;
		 vs_TexLayer = aTextCoordPoint1.z;
	}
	if(gl_VertexID ==1){
		 vs_TexCoord = aTextCoordPoint2.xy;
		 vs_TexLayer = aTextCoordPoint2.z;
	}
	if(gl_VertexID ==2){
		 vs_TexCoord = aTextCoordPoint3.xy;
		 vs_TexLayer = aTextCoordPoint3.z;
	}
	if(gl_VertexID ==3){
		 vs_TexCoord = aTextCoordPoint4.xy;
		 vs_TexLayer = aTextCoordPoint4.z;
	}

	mat4 mat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
	mat =virtualWorldFrame*   sceneFrameInVirtualWorld* ModelMat*mat;
	gl_Position = ProjectionMat*ViewMat*mat*vec4(aPos.x, aPos.y, aPos.z, 1.0);

}