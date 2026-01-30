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

 void main()
{
#if 0
    float gridDensity = 10;        //网格密度
   float lineWidth = 1;
   //float fadeDistance =1;
   vec2 fragpos =vec2(fragPos3D.x/gridDensity, fragPos3D.y/gridDensity);

    vec2 derivative = fwidth(fragpos.xy);

    //当物体距离相机太近，此时导数(变化率)较小,虽然有个数假设
    //x =10.02,距离整数位置=0.02，看着很小，因为导数很小，因此其跨越的屏幕距离是比较大的
    //假设derivative =0.01;
    //grid.x = abs(10.02 - 10.0) / 0.01 = 0.02 / 0.01 = 2.0
    //距离为2个像素，片段A透明。线条显得锐利

    //片段B（远处）：同样位于 x = 10.02。但 derivative.x 很大，比如 0.05。
    //grid.x = abs(10.02 - 10.0) / 0.05 = 0.02 / 0.05 = 0.4
    //距离仅为0.4个像素，片段B半透明。线条显得粗且柔和

    vec2 grid = abs(fract(fragpos.xy - 0.5) - 0.5) / derivative;
    float disToline = min(grid.x, grid.y)/lineWidth;
    
    //如果距离超过一个像素，则不绘制线条，即完全透明
    float alpha = 1.0 - min(disToline, 1);

    FragColor = vec4(224/225.0, 224/225.0, 224/225.0, alpha);
#else
    //获取当前的深度
    float depth = gl_FragCoord.z; //[0-1]
    float normalizeDepth =0;
    float fading = 1.0;

    if(u_cameraType == perspectiveCamera){
        float linearDepth =1.0 /( (1.0/u_far-1.0/u_near)*depth+1.0/u_near);     //范围[n,f]
        normalizeDepth = (linearDepth-u_near)/(u_far-u_near);  //转为0-1
        //如果距离过远，则不绘制网格
        fading = 1-smoothstep(0.5,1.0,normalizeDepth);
    }else{
        normalizeDepth=depth;
    }

    //主网格
    float gridSpace = 4;        //网格间距
    float mainGridDensity = 5*gridSpace;        //网格密度
    float mainGridLineWidth = 1;
    vec2 mainGridFragpos =vec2(fragPos3D.x/mainGridDensity, fragPos3D.z/mainGridDensity);
    vec2 mainGridDerivative = fwidth(mainGridFragpos.xy);

    //次网格
    float  subGridDensity = gridSpace;        //网格密度
    float  subGridLineWidth = 1;
    vec2  subGridFragpos =vec2(fragPos3D.x/subGridDensity, fragPos3D.z/subGridDensity);
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


    #if 1
     if(centerAlpha >0){
        FragColor = vec4(0, 0, 1, centerAlpha*fading);
     }else{
        if(mainGridAlpha<0.01){
            //绘制次网格
            FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.1*subGridAlpha*fading);
        }else{
             if(yaxisAlpha >0.01){
                FragColor = vec4(1, 0, 0, yaxisAlpha*fading);  //会出现锯齿
            }else if(xaxisAlpha >0.01){
                FragColor = vec4(0, 1, 0, xaxisAlpha*fading); //会出现锯齿
            }else{
                FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.3*mainGridAlpha*fading);
            }
        }
    }
    #endif
#endif
}