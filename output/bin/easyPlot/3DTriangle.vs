//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTextureCoord;

out vec4 in_color;
out vec2 in_textureCoord;

flat out int in_instanceID;

uniform mat4 ModelMat;
uniform bool isNdc;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};
void main()
{	 
    in_color = vec4(aColor,1);
	in_textureCoord = aTextureCoord;
    in_instanceID = gl_InstanceID;
    if(isNdc){
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }else{
        gl_Position = ProjectionMat*ViewMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
}