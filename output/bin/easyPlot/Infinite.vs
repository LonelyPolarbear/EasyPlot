//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;         //此处改用ndc坐标

out vec3 fragPos3D;
//计算这几个点在世界坐标系的位置
layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
    mat4 Single_ViewMat;
    mat4 Single_ProjMat;
    mat4 NearplaneFrame;
    mat4 VirtualScreenFrame;
};


/*
* @brief 当绘制无限网格的时候，它表示用户建立的局部坐标系，用户需要在该局部坐标系绘制网格
*             当绘制三维坐标系下的网格时候，该坐标系表示在世界坐标系下的位姿
*             当绘制屏幕坐标系下的网格时候，该坐标系表示在VirtualScreenFrame下的位姿
*
*            当绘制非无限网格，也就是图元内部网格的时候，则假设apos是矩形的四个顶点，它的内部有一个网格坐标系
*            此时gridPlaneMat 表示的是矩形坐标系到网格坐标系的变换
*            
*/
uniform mat4  gridPlaneMat; 
const int CAMERA_MODE_3D_NORMAL=1;
const int CAMERA_MODE_3D_AXIS=2;
const int CAMERA_MODE_2D=3;
uniform int cameraMode;
uniform bool IsInstancedDraw;
uniform mat4 ModelMat;

/**
*@detail 外部提供的待绘制的网格一版有两种类型，一种是无限网格，包络三维空间的无限网格
*              与屏幕二维空间的无限网格，另一种网格则是某个物体内部的网格，网格仅在图元内部显示
*              
*/
uniform bool u_isInfinite;      //true表示无限网格，false表示有限网格
void main()
{	
    //mat4 mat_inv = inverse(gridPlaneMatInWorld); 
    //vec4 worldPos = vec4( aPos,1);
    //fragPos3D =worldPos.xyz;
	//gl_Position = ProjectionMat*ViewMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    if(u_isInfinite){
        vec4 neraPoint = vec4(aPos.x, aPos.y, -1, 1.0);
        vec4 farPoint = vec4(aPos.x, aPos.y, 1, 1.0);

        mat4 proj_inv = inverse(ProjectionMat); 
        mat4 view_inv = inverse(ViewMat); 
        mat4 NearplaneFram_inv = inverse(NearplaneFrame); 
        mat4 VirtualScreenFrame_inv = inverse(VirtualScreenFrame); 
        mat4 gridPlaneMatInWorld_inv = inverse(gridPlaneMat); 

        vec4 neraPoint_world =vec4(0,0,0,1);
         vec4 farPoint_world=vec4(0,0,0,1);
         if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
              neraPoint_world= view_inv *proj_inv*neraPoint;
              farPoint_world = view_inv *proj_inv*farPoint;
        }else if(cameraMode ==CAMERA_MODE_2D){
              neraPoint_world= VirtualScreenFrame_inv*NearplaneFram_inv *proj_inv*neraPoint;
              farPoint_world = VirtualScreenFrame_inv*NearplaneFram_inv *proj_inv*farPoint;
        }
   

        vec4 neraPoint_grid = gridPlaneMatInWorld_inv*neraPoint_world;
        vec4 farPoint_grid =  gridPlaneMatInWorld_inv*farPoint_world;
        neraPoint_grid /=neraPoint_grid.w;            //绘制的网格坐系在的点坐标
        farPoint_grid /=farPoint_grid.w;

        //获取改射线与XOY平面的交点
         vec3 ray =(farPoint_grid - neraPoint_grid).xyz;
         normalize(ray);
         vec3 pA = neraPoint_grid.xyz;
         vec3 pB = farPoint_grid.xyz;
         vec3 intersect_point_grid = vec3(0,0,0);
         if( abs(pA.z- pB.z)>0.01){
            intersect_point_grid= pA+(pB-pA)*(-pA.z)/(pB.z-pA.z);            //交点   
         }
      
        fragPos3D =intersect_point_grid;       //交点

        //顶点位置使用世界坐标系下的位置，避免被视锥裁剪
        vec4 intersect_point_world = gridPlaneMat *vec4(intersect_point_grid.xyz,1);

        // mat4 instancedMat = mat4(1);
         //mat4 ModelMat = mat4(1);
        if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
              gl_Position = ProjectionMat*ViewMat*/*ModelMat*instancedMat**/intersect_point_world;
        }else  if(cameraMode ==CAMERA_MODE_3D_AXIS ){
              gl_Position = vec4(0,0,0,1);
        }else{
              gl_Position = ProjectionMat*NearplaneFrame*VirtualScreenFrame*/*ModelMat*instancedMat**/intersect_point_world;
         }
    }else{
        //有限网格
        vec4 objectPos = vec4(aPos.x,aPos.y,aPos.z,1);
        mat4 gridPlaneMat_inv = inverse(gridPlaneMat); 
        vec4 gridPos = gridPlaneMat_inv * objectPos;
        fragPos3D = gridPos.xyz/gridPos.w;

         //真实的位置
         gl_Position = ProjectionMat*ViewMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);

         if(cameraMode ==CAMERA_MODE_3D_NORMAL ){
              gl_Position = ProjectionMat*ViewMat*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }else  if(cameraMode ==CAMERA_MODE_3D_AXIS ){
              gl_Position = vec4(0,0,0,1);
        }else{
              gl_Position = ProjectionMat*NearplaneFrame*VirtualScreenFrame*ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
         }
    }
}