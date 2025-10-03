//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;

//该着色器用于绘制无限平面，给定的四个点是近平面的NDC坐标
//p0 [-1,-1,-1]
//p1 [1,-1,-1]
//p2 [1,1,-1]
//p3 [-1,1,-1]

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
    mat4 proj_inv = inverse(ProjectionMat);
    mat4 view_inv = inverse(ViewMat);

    vec4 worldNearPosHc= view_inv * proj_inv *vec4(aPos.xy,-1,1);
    vec4 worldFarPosHc= view_inv * proj_inv *vec4(aPos.xy,1,1);
 #if 0   
    vec3 worldNear = worldNearPosHc.xyz/worldNearPosHc.w;                                       //近平面的点
    vec3 worldFar = worldFarPosHc.xyz/worldFarPosHc.w;                                              //远点在世界坐标系的位置
#else
    mat4 mat_inv = inverse(gridPlaneMatInWorld); 
    vec4 worldNear_origin = mat_inv * worldNearPosHc;                                       //近平面的点
    vec4 worldFar_origin = mat_inv * worldFarPosHc;                                              //远点在世界坐标系的位置

    vec3 worldNear = worldNear_origin.xyz / worldNear_origin.w;
    vec3 worldFar = worldFar_origin.xyz / worldFar_origin.w;
#endif
   //计算线与平面的交点
#if 0
   float ty = -worldNear.y / (worldFar.y - worldNear.y);
   fragPos3D = worldNear + ty * (worldFar - worldNear);
   gl_Position = ProjectionMat*ViewMat*vec4(fragPos3D.x, fragPos3D.y, fragPos3D.z, 1.0);
   fragPos3D = fragPos3D.xzy;

 #else
    float ty = -worldNear.z / (worldFar.z - worldNear.z);
   fragPos3D = worldNear + ty * (worldFar - worldNear);

   //得到交点后，交点的位置转换到需要绘制网格的平面上
   vec4 worldPos = gridPlaneMatInWorld * vec4(fragPos3D, 1.0);
   vec3 worldPos_ = worldPos.xyz / worldPos.w;
	gl_Position = ProjectionMat*ViewMat*vec4(worldPos_.x, worldPos_.y, worldPos_.z, 1.0);

    //fragPos3D = fragPos3D.xzy;

#endif
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}