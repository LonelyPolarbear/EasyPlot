#version 430 core
 
 in vec3 fragPos3D;
 flat in vec3 vs_origin;
 out vec4 FragColor;
 uniform float  gridSpace;
uniform  int  gridNum;
uniform bool showAxis;
 void main()
{   
    vec3 fragPos3D_ = fragPos3D+vs_origin;
   
    //主网格
    //float gridSpace = 4;        //网格间距
    float mainGridDensity = gridNum*gridSpace;        //网格密度
    float mainGridLineWidth = 1;
    vec2 mainGridFragpos =vec2(fragPos3D_.x/mainGridDensity, fragPos3D_.y/mainGridDensity);
    vec2 mainGridDerivative = abs(fwidth(mainGridFragpos.xy));
    //vec2 mainGridDerivative = abs(fwidth(  fract(mainGridFragpos.xy - 0.5) - 0.5  ));

    //次网格
    float  subGridDensity = gridSpace;        //网格密度
    float  subGridLineWidth = 1;
    vec2  subGridFragpos =vec2(fragPos3D_.x/subGridDensity, fragPos3D_.y/subGridDensity);
    vec2  subGridDerivative = fwidth(subGridFragpos.xy);
    //vec2  subGridDerivative = abs(fwidth( fract(subGridFragpos.xy - 0.5) - 0.5 ));

    /*
     if(  (fragPos3D_.x <0.0 && abs(fragPos3D_.x)>abs(subGridDerivative.x) )           ||
           (fragPos3D_.y <0.0 && abs(fragPos3D_.y)>abs(subGridDerivative.y))
         )
        discard;
     */

    //当物体距离相机太近，此时导数(变化率)较小,虽然有个数假设
    //x =10.02,距离整数位置=0.02，看着很小，因为导数很小，因此其跨越的屏幕距离是比较大的
    //假设derivative =0.01;
    //grid.x = abs(10.02 - 10.0) / 0.01 = 0.02 / 0.01 = 2.0
    //距离为2个像素，片段A透明。线条显得锐利

    //片段B（远处）：同样位于 x = 10.02。但 derivative.x 很大，比如 0.05。
    //grid.x = abs(10.02 - 10.0) / 0.05 = 0.02 / 0.05 = 0.4
    //距离仅为0.4个像素，片段B半透明。线条显得粗且柔和

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

 #if 0
     if(centerAlpha >0){
        FragColor = vec4(0, 0, 1, centerAlpha);
     }else{
        if(mainGridAlpha<0.01){
            //绘制次网格
            FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.1*subGridAlpha);
        }else{
             if(abs(mainGridFragpos.y/ mainGridDerivative.y)<1){
                FragColor = vec4(1, 0, 0, 1);
            }else if(abs(mainGridFragpos.x/ mainGridDerivative.x)<1){
                FragColor = vec4(0, 1, 0, 1);
            }else{
                FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.25*mainGridAlpha);
            }
        }
    }
#else

    float xaxisAlpha = 1.0 - smoothstep(0.4, 1.0, abs(centerDist.x));
    float yaxisAlpha = 1.0 - smoothstep(0.4, 1.0, abs(centerDist.y));

    if(showAxis){
        if(centerAlpha >0){
            FragColor = vec4(0, 0, 1, centerAlpha);
         }else{
            if(mainGridAlpha<0.01){
                //绘制次网格
                FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.1*subGridAlpha);
            }else{

                if(yaxisAlpha >0.001){
                    FragColor = vec4(1, 0, 0, yaxisAlpha);  //会出现锯齿
                }else if(xaxisAlpha >0.001){
                    FragColor = vec4(0, 1, 0, xaxisAlpha); //会出现锯齿
                }else{
                    FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.3*mainGridAlpha);
                }
            }
        }
    }
    else
        {
            if(mainGridAlpha<0.01){
                //绘制次网格
                FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.1*subGridAlpha);
            }else{
                FragColor = vec4(224/225.0, 224/225.0, 224/225.0, 0.3*mainGridAlpha);
            }
        }
#endif
}