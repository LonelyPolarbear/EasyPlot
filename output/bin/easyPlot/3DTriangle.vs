//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTextureCoord;

layout (location = 4) in vec4 aInstanceMatC0l1;         //实例化渲染第一列
layout (location = 5) in vec4 aInstanceMatC0l2;         //实例化渲染第二列
layout (location = 6) in vec4 aInstanceMatC0l3;         //实例化渲染第三列
layout (location = 7) in vec4 aInstanceMatC0l4;         //实例化渲染第四列

out vec4 in_color;
out vec2 in_textureCoord;

flat out int in_instanceID;

uniform mat4 ModelMat;
uniform bool isNdc;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
    mat4 Single_ViewMat;
    mat4 Single_ProjMat;
};

//uniform mat4 Single_ViewMat;
//uniform mat4 Single_ProjMat;
uniform bool UseNoramlCamera;
uniform bool IsInstancedDraw;
void main()
{	 
    in_color = vec4(aColor,1);
	in_textureCoord = aTextureCoord;
    in_instanceID = gl_InstanceID;
    mat4 instancedMat = mat4(1);
    if(IsInstancedDraw){
        instancedMat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
    }
    
    if(isNdc){
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }else{
        if(UseNoramlCamera){
            gl_Position = ProjectionMat*ViewMat*instancedMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }else{
            gl_Position = Single_ProjMat*Single_ViewMat*instancedMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    }
}