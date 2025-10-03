#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 ModelMat;
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
	gl_Position = ProjectionMat*ViewMat* virtualWorldFrame*sceneFrameInVirtualWorld*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}