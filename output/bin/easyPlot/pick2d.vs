//顶点着色器
#version 430 core
layout (location = 0) in vec3 aPos;


uniform mat4 ModelMat;
//uniform mat4 ViewMat;
//uniform mat4 ProjectionMat;

layout (std140, binding = 4) uniform Matrices2d
{
    mat4 virtualWorldFrame;
	mat4 sceneFrameInVirtualWorld;
};

uniform bool isInstance;
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

out vec3 scenePos;

layout (std140, binding = 1) uniform Matrices
{
    mat4 ViewMat;
    mat4 ProjectionMat;
};

uniform vec2 screenSize;

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
    mat4 mat = mat4(1.0);
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
	}
	else if(PositionType == sceneScreen_center){
		mat4  posture= sceneFrameInVirtualWorld *ModelMat*ModelMatSelf;

		mat4 sceneScreenFrameToVirtualWorldFrame = mat4(1.0);
		if(Orientation == left_bottom){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == left_top){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(-0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_top){
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,0.5*screenSize.y,   0  ,1);
		}
		else if(Orientation == right_bottom){
			// X Z反向
			sceneScreenFrameToVirtualWorldFrame[3] = vec4(0.5*screenSize.x,-0.5*screenSize.y,   0  ,1);
		}

		mat4 pos =sceneScreenFrameToVirtualWorldFrame* ModelMat*ModelMatSelf;
		mat4 pos_posture =mat4(posture[0],posture[1],posture[2],pos[3]);

		mat =virtualWorldFrame*pos_posture*mat;
	}

	//gl_Position = ProjectionMat*ViewMat* virtualWorldFrame*   sceneFrameInVirtualWorld*ModelMat*ModelMatSelf* vec4(aPos.x, aPos.y, aPos.z, 1.0);

	gl_Position = ProjectionMat*ViewMat*mat*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}