#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aInstanceMatC0l1;
layout (location = 2) in vec4 aInstanceMatC0l2;
layout (location = 3) in vec4 aInstanceMatC0l3;
layout (location = 4) in vec4 aInstanceMatC0l4;

layout (location = 5) in vec3 aTextCoordPoint1;   //纹理坐标
layout (location = 6) in vec3 aTextCoordPoint2;   //纹理坐标
layout (location = 7) in vec3 aTextCoordPoint3;   //纹理坐标
layout (location = 8) in vec3 aTextCoordPoint4;   //纹理坐标

uniform bool isInstance;
uniform mat4 ModelMat;										//父类的变换矩阵
uniform mat4 ModelMatSelf;									//本类的变换矩阵

/*
	local_complete,					//所有的点都在本地坐标系中，包括中心点
	local_center,						//只有中心点在本地坐标系中
	sceneScreen_complete,		//场景屏幕坐标系，同时意味着固定大小
	sceneScreen_center,			//场景屏幕坐标系，只有中心点在屏幕坐标系中
*/
const int local_complete =0;
const int local_center =1;
const int sceneScreen_complete =2;
const int sceneScreen_center =3;
uniform int PositionType;	

/*
enum class LIB05_SHAPE_API Orientation {
		left_top,
		left_bottom,
		right_top,
		right_bottom
	};
*/
const int left_top =0;
const int left_bottom =1;
const int right_top =2;
const int right_bottom =3;
uniform int Orientation;		//只用于 sceneScreen_complete和 sceneScreen_center


out vec2 vs_TexCoord;
flat out float vs_TexLayer;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};

layout (std140, binding = 4) uniform Matrices2d
{
    mat4 virtualWorldFrame;
	mat4 sceneFrameInVirtualWorld;																	
};

layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};


mat4 removeScale(mat4 m) {
    // 保存平移分量
    vec4 translation = m[3];
    
    // 单位化前三列（方向向量）
    vec3 xAxis = normalize(m[0].xyz);
    vec3 yAxis = normalize(m[1].xyz);
    vec3 zAxis = normalize(m[2].xyz);
    
    // 重新构建矩阵，保持平移不变
    return mat4(
        vec4(xAxis, 0.0),
        vec4(yAxis, 0.0),
        vec4(zAxis, 0.0),
        translation
    );
}

void main()
{	
	if(gl_VertexID ==0){
		 vs_TexCoord = aTextCoordPoint1.xy;
		 vs_TexLayer = aTextCoordPoint1.z;
	}
	if(gl_VertexID ==1){
		 vs_TexCoord = aTextCoordPoint2.xy;
		 vs_TexLayer = aTextCoordPoint2.z;
	}
	if(gl_VertexID ==2){
		 vs_TexCoord = aTextCoordPoint3.xy;
		 vs_TexLayer = aTextCoordPoint3.z;
	}
	if(gl_VertexID ==3){
		 vs_TexCoord = aTextCoordPoint4.xy;
		 vs_TexLayer = aTextCoordPoint4.z;
	}

	mat4 mat = mat4(aInstanceMatC0l1, aInstanceMatC0l2, aInstanceMatC0l3, aInstanceMatC0l4);
	//mat4 ModelMatSelf = mat4(1.0);
	//表示当前输入的顶点坐标是场景屏幕坐标系中的点，需要转换到虚拟世界坐标系中
	//sceneFrameInVirtualWorld将不再使用，手动构建一个屏幕场景坐标系到虚拟世界坐标系的转换矩阵
	if(PositionType == sceneScreen_complete){
		mat4 sceneScreenFrameToVirtualWorldFrame = mat4(1.0);
		if(Orientation == left_bottom){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == left_top){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_top){
			// X Y反向
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_bottom){
			// X Z反向
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}

		//mat = virtualWorldFrame*   sceneScreenFrameToVirtualWorldFrame* ModelMat*ModelMatSelf*mat;
		//去除场景和父类的变换影响，即可保证顶点始终是在屏幕坐标系中
		mat = virtualWorldFrame*   sceneScreenFrameToVirtualWorldFrame*ModelMatSelf*mat;
	}
	else if(PositionType == local_complete){
		mat =virtualWorldFrame*   sceneFrameInVirtualWorld* ModelMat*ModelMatSelf*mat;
	}
	else if(PositionType == local_center){
		mat4 localToWorld =   sceneFrameInVirtualWorld*ModelMat*ModelMatSelf;
		mat4 removeScale_sceneFrameInVirtualWorld =removeScale(localToWorld);
		mat4  posture= removeScale_sceneFrameInVirtualWorld ;
		mat4  pos=  localToWorld;
		mat4 pos_posture =mat4(posture[0],posture[1],posture[2],pos[3]);

		mat =virtualWorldFrame* pos_posture*/*ModelMatSelf**/mat;
	}else if(PositionType == sceneScreen_center){
		mat4  posture= sceneFrameInVirtualWorld *ModelMat*ModelMatSelf;

		mat4 sceneScreenFrameToVirtualWorldFrame = mat4(1.0);
		if(Orientation == left_bottom){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == left_top){
			//sceneScreenFrameToVirtualWorldFrame[1] = vec4(0,-1,0,0);		//y反向
			//sceneScreenFrameToVirtualWorldFrame[2] = vec4(0,0,-1,0);		//z反向
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_top){
			//sceneScreenFrameToVirtualWorldFrame[0] = vec4(-1,0,0,0);
			//sceneScreenFrameToVirtualWorldFrame[1] = vec4(0,-1,0,0);
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_bottom){
			// X Z反向
			//sceneScreenFrameToVirtualWorldFrame[0] = vec4(-1,0,0,0);
			//sceneScreenFrameToVirtualWorldFrame[2] = vec4(0,0,-1,0);
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}

		mat4 pos =sceneScreenFrameToVirtualWorldFrame* ModelMat*ModelMatSelf;
		mat4 pos_posture =mat4(posture[0],posture[1],posture[2],pos[3]);

		mat =virtualWorldFrame*pos_posture*mat;
	}

	gl_Position = ProjectionMat*ViewMat*mat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}