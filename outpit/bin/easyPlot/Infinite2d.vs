//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 fragPos3D;
flat out vec3 vs_origin;
uniform vec2 Origin;    //网格的原点，用于网格原点不在中心时候的处理
//计算这几个点在世界坐标系的位置
layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};


layout (std140, binding = 2) uniform ubo_vs
{
    vec2 screenSize;																				                                                                    //屏幕大小
    vec2 setRealTimeMousePos;																				                                                //鼠标实时位置，用于预选
};


layout (std140, binding = 4) uniform Matrices2d
{
    mat4 virtualWorldFrame;
	mat4 sceneFrameInVirtualWorld;																			                                                     //鼠标实时位置，用于预选
};

uniform mat4 ModelMat;
uniform bool isScreenGrid;
uniform mat4 ObjectMat;                                                                                                                                                 //物体局部坐标系在网格坐标下的位姿

void main()
{	
    if(isScreenGrid)
    {   
        //屏幕网格,也就是场景坐标系下的网格
        vec4  virtualWorldFramepos = vec4(aPos.x*0.5*screenSize.x,aPos.y*0.5*screenSize.y,0,1);                             //当前点在virtualWorldFrame的位置
        mat4 sceneFrameInVirtualWorld_inv = inverse(sceneFrameInVirtualWorld);
        vec4  woldFramepos = virtualWorldFrame * virtualWorldFramepos;                                                                  //当前点在sceneFrame的位置
        vec4  sceneFramepos = sceneFrameInVirtualWorld_inv * virtualWorldFramepos;                                              //当前点在sceneFrame的位置
        fragPos3D = sceneFramepos.xyz/sceneFramepos.w;
	    gl_Position = ProjectionMat*ViewMat*woldFramepos;
    }else{
        //某个图元内的网格，网格是依据物体的坐标系来定义的

        vec4 objectPos = vec4(aPos.x,aPos.y,aPos.z,1);
        vec4 gridPos = ObjectMat * objectPos;
        fragPos3D = gridPos.xyz/gridPos.w;

        vec4 tmp = ObjectMat* vec4(Origin.x,Origin.y,aPos.z,1);
        vs_origin = tmp.xyz/tmp.w;

        gl_Position = ProjectionMat*ViewMat*virtualWorldFrame*sceneFrameInVirtualWorld* ModelMat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
}