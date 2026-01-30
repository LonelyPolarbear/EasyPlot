//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 fragPos3D;
//计算这几个点在世界坐标系的位置
layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};

uniform mat4  gridPlaneMatInWorld;  //默认外部给单位矩阵
void main()
{	

    mat4 mat_inv = inverse(gridPlaneMatInWorld); 
    vec4 worldPos = mat_inv*vec4( aPos,1);
    fragPos3D =worldPos.xyz;
  
   //得到交点后，交点的位置转换到需要绘制网格的平面上
	gl_Position = ProjectionMat*ViewMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}