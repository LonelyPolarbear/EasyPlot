//片段着色器
#version 430 core
layout (location =0) out vec4 FragColor;

//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																									//屏幕大小,是当前render的窗口大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
	vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height
};


layout(binding = 3) uniform usampler2DMS IdSample;	                                    //物体ID纹理信息
layout(binding = 4) uniform usampler2DMS ColorSample;	
layout(binding = 5) uniform sampler2D DepthSample;	

//顶点属性输入
in vec4 in_color;
in vec2 in_textureCoord;
flat in int in_instanceID;
/**************************************************************************************************************/
//如果coord处有一个样本点的id是0，说明就是边界
bool haObjectZero(ivec2 coordInput,ivec2 dir, int samples,int step)
{
    for(int i=1;i<=step;++i){
        ivec2 coord = coordInput+dir*step;
        if (coord.x < 0 || coord.y < 0 || coord.x >= int(screenSize.x) || coord.y >= int(screenSize.y))
            return false;
    
        for (int j = 0; j < samples; ++j) {
            uint stencilVal = uint(texelFetch(IdSample, coord, j).g);
            if (stencilVal == 0u)
                return true;
        }
    }
    return false;
}

#if 1
void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    const int samples = 8;   // 多重采样样本数

    //采样深度，和当前深度缓冲区深度比较
    float depth =0;
    depth = texture(DepthSample, coord).r;
    #if 0
     for (int i = 0; i < samples; ++i) {
       float d = texelFetch(DepthSample, coord, i).r;
       depth +=d;
    }
    depth /=float(samples);
    #endif

     //FragColor = vec4(depth,depth,depth, 1.0);   //边缘
     //return;
     if(gl_FragCoord.z > depth){
        discard;
     }
    
    // 统计每个 ID 出现的次数
   int count =0;
    for (int i = 0; i < samples; ++i) {
        uint id = texelFetch(IdSample, coord, i).g;
        if(id !=0){
            count++;
        }
    }

    vec3 color = vec3(0,0,0);
    for (int i = 0; i < samples; ++i) {
        color.rgb += texelFetch(ColorSample, coord, i).rgb;
    }

    color.rgb = color.rgb/float(samples);
    
    float edgeStrength = float(count) / float(samples);
    vec3 edgeColor =vec3(1,1,0);
    if (count<samples) {
        //存在值为0的样本点，说明是0
        vec3 finalColor = mix(color, edgeColor, edgeStrength);
        FragColor = vec4(finalColor,1);   // 黄色，强度为透明度
    } else {
        //非边缘
        bool leftZero  = haObjectZero(coord , ivec2(-1, 0), samples,3);
        bool rightZero = haObjectZero(coord , ivec2( 1, 0), samples,3);
        bool downZero  = haObjectZero(coord , ivec2( 0,-1), samples,3);
        bool upZero    = haObjectZero(coord , ivec2( 0, 1), samples,3);
         if (leftZero || rightZero || downZero || upZero) {
            //说明是靠近边缘
            FragColor = vec4(edgeColor, 1.0);   //边缘
        }else{
            discard;
        }
    }
}
#endif