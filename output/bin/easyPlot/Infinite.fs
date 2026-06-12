#version 430 core
 
 in vec3 fragPos3D;

 out vec4 FragColor;

 //透视投影相机信息

layout (std140, binding = 5) uniform ubo_cameraData
{
    uniform int u_cameraType;
    uniform float u_near;
    uniform float u_far;
    uniform int bak;
};

const int orothoCamera = 1; //正交投影相机
const int perspectiveCamera = 2; //透视投影相机


uniform float gridSpace;                          //网格间距  4
uniform float mainGridDensity;              //主网格密度 5*gridSpace
uniform float subGridDensity;                 //次网格密度 gridSpace
uniform float mainGridLineWidth;         //主网格宽度  2
uniform float subGridLineWidth;           //主网格宽度  1
uniform float mainGridAlphaFactor;      //主网格宽度  累加透明系数 0.3
uniform float subGridAlphaFactor;        //次网格宽度  累加透明系数 0.3
 void main()
{
    //获取当前的深度
    float depth = gl_FragCoord.z; //[0-1]
    float normalizeDepth =0;
    float fading = 1.0;
    //vec3 gridColor = vec3(224/225.0, 224/225.0, 224/225.0);
    vec3 gridColor = vec3(0.804, 0.788, 0.788);

    if(u_cameraType == perspectiveCamera){
        float linearDepth =1.0 /( (1.0/u_far-1.0/u_near)*depth+1.0/u_near);     //范围[n,f]
        normalizeDepth = (linearDepth-u_near)/(u_far-u_near);  //转为0-1
        //如果距离过远，则不绘制网格
        fading = 1-smoothstep(0.5,1.0,normalizeDepth);
    }else{
        normalizeDepth=depth;
    }

    //主网格
    //float gridSpace = 4;        //网格间距
    //float mainGridDensity = 5*gridSpace;        //网格密度
   // float mainGridLineWidth = 2;
    vec2 mainGridFragpos =vec2(fragPos3D.x/mainGridDensity, fragPos3D.y/mainGridDensity);
    vec2 mainGridDerivative = fwidth(mainGridFragpos.xy);

    //次网格
    //float  subGridDensity = gridSpace;        //网格密度
    //float  subGridLineWidth = 1;
    vec2  subGridFragpos =vec2(fragPos3D.x/subGridDensity, fragPos3D.y/subGridDensity);
    vec2  subGridDerivative = fwidth(subGridFragpos.xy);

    vec2 mainGridDisToline_ = abs(fract(mainGridFragpos.xy - 0.5) - 0.5) / mainGridDerivative;
    float mainGridDisToline = min(mainGridDisToline_.x, mainGridDisToline_.y)/mainGridLineWidth;

    vec2 subGridDisToline_ = abs(fract(subGridFragpos.xy - 0.5) - 0.5) / subGridDerivative;
    float subGridDisToline = min(subGridDisToline_.x, subGridDisToline_.y)/subGridLineWidth;
    
    //如果距离超过一个像素，则不绘制线条，即完全透明
    float mainGridAlpha = 1.0 - min(mainGridDisToline, 1);
    float subGridAlpha = 1.0 - min(subGridDisToline, 1);

    // 计算到中心的距离（用于中心区域抗锯齿）
    vec2 centerDist = abs(mainGridFragpos.xy) / mainGridDerivative;
    float centerAlpha = 1.0 - smoothstep(2.0, 3.0, max(centerDist.x, centerDist.y));

    float xaxisAlpha = 1.0 - smoothstep(0.5, 1.0, abs(centerDist.x));
    float yaxisAlpha = 1.0 - smoothstep(0.5, 1.0, abs(centerDist.y));

     if(centerAlpha >0){
        FragColor = vec4(0, 0, 1, centerAlpha*fading);
     }else{
        if(mainGridAlpha<0.01){
            //绘制次网格
            FragColor = vec4(gridColor, subGridAlphaFactor*subGridAlpha*fading);
        }else{
             if(yaxisAlpha >0.01){
                FragColor = vec4(1, 0, 0, yaxisAlpha*fading);  //会出现锯齿
            }else if(xaxisAlpha >0.01){
                FragColor = vec4(0, 1, 0, xaxisAlpha*fading); //会出现锯齿
            }else{
                FragColor = vec4(gridColor, mainGridAlphaFactor*mainGridAlpha*fading);
            }
        }
    }
}