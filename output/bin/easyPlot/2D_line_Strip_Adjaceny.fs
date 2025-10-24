//片段着色器
#version 430 core
 out vec4 FragColor;
 in vec2 texCoord;		//FSAA
 layout(std430, binding = 0) buffer MySSBO_Block  {
    vec4 data[]; // 动态大小数组
}MySSBO;

 layout(std430, binding = 1) buffer MySSBO_Block2  {
    int size; // 动态大小数组
    float data[]; // 动态大小数组
}MySSBO_Len;        //记录每一个图元的长度

//ubo，存储所有着色器需要的常量，且常量与具体模型无关，如当前鼠标的位置、屏幕大小等
layout (std140, binding = 2) uniform ubo_fs
{
    vec2 screenSize;																				//屏幕大小
    vec2 setRealTimeMousePos;																				//鼠标实时位置，用于预选
    vec4 sceneRect;																									//场景矩形,当前场景的位置，x y width height,暂时不用
};

const int COLOMODE_SINGLECOLOR = 1;
const int COLOMODE_VERTEXCOLOR = 2;
const int COLOMODE_FACECOLOR = 3;												//每个片元一个颜色
const int COLOMODE_TEXTURECOLOR = 4;										//每个片元一个颜色
const int COLOMODE_SELECT_TEST_COLOR = 5;							//每个片元一个颜色

const int PEN_STYLE_SOLID = 0;
const int PEN_STYLE_DASH = 1;
const int PEN_STYLE_DOT = 2;
const int PEN_STYLE_DASHDOT = 3;
const int PEN_STYLE_DASHDOTDOT = 4;


const int POLYGONMODE_POINT = 1;
const int POLYGONMODE_LINE = 2;
const int POLYGONMODE_FILL = 3;

uniform int polygonMode;

uniform int colorMode;																		//颜色模式
uniform vec4 singleColor;																	//单色
uniform vec4 selectTestColor;															//选择测试颜色
uniform vec4 preSelectColor;															//预选颜色

uniform uint objectID;																		//当前对象ID
uniform int penStyle;																		    //画笔样式

layout(binding = 1) uniform sampler2D depthSample;					//深度纹理，暂未用到
layout(binding = 2) uniform usampler2D objectIdSample;				//对象ID纹理(来自FBO)，用于预选


//顶点属性输入
in vec4 in_color;
flat in float SCalelineWidth;               //SCalelineWidth可能小于2
flat in float SCalelineWidthInGs;       //用于GS中计算线宽，受缩放影响，但是在几何着色器中使用的线宽 >=2
flat in float gs_lineLength;                 //线的长度，是屏幕空间的长度，不受缩放影响

float paintDashLine(float distancex, float dashLength, float dashSpace,float alpha){
    float result = alpha;
    float period = dashLength+dashSpace;
    float modDistance = mod(distancex, period);  // 周期内的相对距离

    if(modDistance < period && modDistance > dashLength)
    {
        result = 0.0;
    }
    return result;
}

float paintDotLine(float distancex, float dotLength, float dotSpace,float alpha){
    float result = alpha;
    float period = dotLength + dotSpace;
    float modDistance = mod(distancex, period);  // 周期内的相对距离

    if(modDistance > dotLength)
    {
        result = 0.0;
    }
    return result;
}

float paintDashDotLine(float distancex, float dashLength, float dashSpace, float dotLength,float alpha){
    float dotSpace = dashSpace;
    float result = alpha;
    float period = dashLength + dashSpace + dotLength + dotSpace;
    float modDistance = mod(distancex, period);  // 周期内的相对距离

    //|----    -  |----    -  |----    -  
    //dashLength  dashSpace dotLength  dotSpace
    if(  modDistance>dashLength && modDistance < dashLength + dashSpace)
    {
        result = 0;
    }

     if(modDistance > dashLength + dashSpace+ dotLength)
    {
        result = 0;
    }
    
    return result;
}

float paintDashDotDotLine(float distancex, float dashLength, float dashSpace, float dotLength,float alpha){

    float result = alpha;
    float period = dashLength + 3*dashSpace + 2*dotLength;
    float modDistance = mod(distancex, period);  // 周期内的相对距离

    //|----   -  - |----   -  - |----   -  - 
    //dashLength  dashSpace dotLength  dotSpace
    float first = dashLength;
    float second = dashLength + dashSpace + dotLength;
    float third = dashLength + 2*dashSpace +2*dotLength;
    
    if(  modDistance>first && modDistance < first + dashSpace)
    {
        result = 0;
    }

     if(modDistance > second  && modDistance <second+dashSpace)
    {
        result = 0;
    }

    if(modDistance > third )
    {
        result = 0;
    }
    
    return result;
}

void main()
{	
	if(colorMode == COLOMODE_SINGLECOLOR){
         float minLineWidth =1;
         float ratio =min(SCalelineWidth/ minLineWidth,1);       //alpha需要叠加这个值，从而让线看着更透明，从而表现线的缩放效果
        // float ratio = SCalelineWidth;
        float alpha =1;
#if 0
         float len =  SCalelineWidthInGs;      
         float uHalfWidth  = 0.5 * len;
         vec2 derivative = fwidth(texCoord.xy);
         float uAARadius =  clamp(uHalfWidth,0.5f,1.f); 
         float distancey = abs(texCoord.y - 0.5) * len;
         alpha = 1.0 - smoothstep(uHalfWidth-uAARadius, uHalfWidth+uAARadius*0.3, distancey);
#else
         float uHalfWidth  = 0.5;
         vec2 derivative = fwidth(texCoord.xy);
         float uAARadius = derivative.y;
         float distancey = abs(texCoord.y - 0.5);
         if(SCalelineWidthInGs <=1.001){
            //线宽太小，不用AA
            //alpha = 1.0 - smoothstep(uHalfWidth-uAARadius, uHalfWidth+uAARadius*2, distancey);
            alpha =1;
         }
         else
            alpha = 1.0 - smoothstep(uHalfWidth-uAARadius, uHalfWidth+uAARadius*0.3, distancey);
#endif
          float distancex = texCoord.x * gs_lineLength;  //当前像素在当前图元的长度
          distancex = distancex+MySSBO_Len.data[gl_PrimitiveID];
          float dashLength = 10;
          float dashSpace = 5;
          float dotLength = 5;
          float dotSpace = 10;
          switch(penStyle){
              case PEN_STYLE_SOLID:
                  break;
              case PEN_STYLE_DASH:
                  alpha = paintDashLine(distancex, dashLength, dashSpace, alpha);  
                  break;
              case PEN_STYLE_DOT:
                  alpha = paintDotLine(distancex, dotLength, dotSpace, alpha);  
                  break;
              case PEN_STYLE_DASHDOT:
                  alpha = paintDashDotLine(distancex, dashLength, dotSpace, dotSpace, alpha);  
                  break;
              case PEN_STYLE_DASHDOTDOT:
                  alpha = paintDashDotDotLine(distancex, dashLength, dashSpace, 2, alpha);  
                  break;
              default:
                  break;
          }

        FragColor = vec4(singleColor.rgb,  alpha*ratio);
    }
}