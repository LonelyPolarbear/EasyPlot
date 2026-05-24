//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;         //此处改用ndc坐标

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
#if 1
    mat4 mat_inv = inverse(gridPlaneMatInWorld); 
    vec4 worldPos = mat_inv*vec4( aPos,1);
    fragPos3D =worldPos.xyz;
  
   //得到交点后，交点的位置转换到需要绘制网格的平面上
	gl_Position = ProjectionMat*ViewMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
#else
/*
    vec4 neraPoint = vec4(aPos.x, aPos.y, -1, 1.0);
    vec4 farPoint = vec4(aPos.x, aPos.y, 1, 1.0);

    mat4 proj_inv = inverse(ProjectionMat); 
    mat4 view_inv = inverse(ViewMat); 
    vec4 neraPoint_world = view_inv *proj_inv*neraPoint;
    vec4 farPoint_world = view_inv *proj_inv*farPoint;
    neraPoint_world /=neraPoint_world.w;
    farPoint_world /=farPoint_world.w;
    //获取改射线与平面的交点
     vec3 ray =(farPoint_world - neraPoint_world).xyz;
     normalize(ray);
     vec3 pA = neraPoint_world.xyz;
     vec3 pB = farPoint_world.xyz;
     vec3 m = vec3(0,0,0);
     if(pA.y- pB.y !=0){
        m= pA+(pB-pA)*(-pA.y)/(pB.y-pA.y);            //交点   
     }
      
    gl_Position = ProjectionMat*ViewMat*vec4(m, 1.0);
    fragPos3D =m;
    */
#endif
}