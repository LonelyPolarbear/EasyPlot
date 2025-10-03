//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aInstanceMatC0l1;
layout (location = 2) in vec4 aInstanceMatC0l2;
layout (location = 3) in vec4 aInstanceMatC0l3;
layout (location = 4) in vec4 aInstanceMatC0l4;


uniform mat4 ModelMat;
uniform bool isInstance;

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
	if(isInstance)
	{	
		mat4 mat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
		mat =virtualWorldFrame*   sceneFrameInVirtualWorld* ModelMat*mat;
		gl_Position = ProjectionMat*ViewMat*mat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}else{

		gl_Position = ProjectionMat*ViewMat* virtualWorldFrame* sceneFrameInVirtualWorld* ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
}