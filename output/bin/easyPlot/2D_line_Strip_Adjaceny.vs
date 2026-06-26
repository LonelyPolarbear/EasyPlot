//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;                               //位置
layout (location = 1) in vec3 aNormal;                         //法线
layout (location = 2) in vec3 aColor;                            //颜色
layout (location = 3) in vec2 aTextureCoord;              //纹理坐标

layout (location = 4) in vec4 aInstanceMatC0l1;         //实例化渲染第一列
layout (location = 5) in vec4 aInstanceMatC0l2;         //实例化渲染第二列
layout (location = 6) in vec4 aInstanceMatC0l3;         //实例化渲染第三列
layout (location = 7) in vec4 aInstanceMatC0l4;         //实例化渲染第四列

out vec4 in_color;
out vec2 in_textureCoord;
out vec4 vWorldPos;//主要给几何着色器传递

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

//uniform bool UseNoramlCamera;
uniform bool IsInstancedDraw;

const int CAMERA_MODE_3D_NORMAL=1;
const int CAMERA_MODE_3D_AXIS=2;
const int CAMERA_MODE_2D=3;
uniform int cameraMode;

void main()
{	 
    in_color = vec4(aColor,1);
	in_textureCoord = aTextureCoord;
    in_instanceID = gl_InstanceID;
    mat4 instancedMat = mat4(1);
    if(IsInstancedDraw){
        instancedMat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
    }
    
    vec4 world_pos = ModelMat*instancedMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);

    if(isNdc){
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }else{
        if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
    
            gl_Position = ProjectionMat*ViewMat*world_pos;
        }else  if(cameraMode ==CAMERA_MODE_3D_AXIS ){
            
            gl_Position = Single_ProjMat*Single_ViewMat*world_pos;
        }else{
             
            gl_Position = ProjectionMat*NearplaneFrame*VirtualScreenFrame*world_pos;
        }
    }
    vWorldPos = world_pos;
}