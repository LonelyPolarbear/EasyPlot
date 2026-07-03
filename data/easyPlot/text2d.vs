#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTextureCoord;

layout (location = 4) in vec4 aInstanceMatC0l1;         //实例化渲染第一列
layout (location = 5) in vec4 aInstanceMatC0l2;         //实例化渲染第二列
layout (location = 6) in vec4 aInstanceMatC0l3;         //实例化渲染第三列
layout (location = 7) in vec4 aInstanceMatC0l4;         //实例化渲染第四列

layout (location = 8) in vec3 aTextCoordPoint1;   //纹理坐标
layout (location = 9) in vec3 aTextCoordPoint2;   //纹理坐标
layout (location = 10) in vec3 aTextCoordPoint3;   //纹理坐标
layout (location = 11) in vec3 aTextCoordPoint4;   //纹理坐标

out vec4 in_color;
out vec2 in_textureCoord;
flat out float vs_TexLayer;         //纹理层级

flat out int in_instanceID;

uniform mat4 ModelMat;
uniform bool isNdc;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
    mat4 Single_ViewMat;
    mat4 Single_ProjMat;
    mat4 NearplaneFrame;
    mat4 VirtualScreenFrame;
};

const int CAMERA_MODE_3D_NORMAL=1;
const int CAMERA_MODE_3D_AXIS=2;
const int CAMERA_MODE_2D=3;
uniform int cameraMode;

uniform bool IsInstancedDraw;
void main()
{	 
    if(gl_VertexID ==0){
		 in_textureCoord = aTextCoordPoint1.xy;
		 vs_TexLayer = aTextCoordPoint1.z;
	}
	if(gl_VertexID ==1){
		 in_textureCoord = aTextCoordPoint2.xy;
		 vs_TexLayer = aTextCoordPoint2.z;
	}
	if(gl_VertexID ==2){
		 in_textureCoord = aTextCoordPoint3.xy;
		 vs_TexLayer = aTextCoordPoint3.z;
	}
	if(gl_VertexID ==3){
		 in_textureCoord = aTextCoordPoint4.xy;
		 vs_TexLayer = aTextCoordPoint4.z;
	}

    in_color = vec4(aColor,1);
	//in_textureCoord = aTextureCoord;
    in_instanceID = gl_InstanceID;
    mat4 instancedMat = mat4(1);
    if(IsInstancedDraw){
        instancedMat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
    }
    
    if(isNdc){
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }else{
        if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
            gl_Position = ProjectionMat*ViewMat*ModelMat*instancedMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }else  if(cameraMode ==CAMERA_MODE_3D_AXIS ){
            gl_Position = Single_ProjMat*Single_ViewMat*ModelMat*instancedMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }else{
            gl_Position = ProjectionMat*NearplaneFrame*VirtualScreenFrame*ModelMat*instancedMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    }
}