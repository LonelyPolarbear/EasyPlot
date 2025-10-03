#version 430 core
 out uvec4 FragColor;
uniform unsigned int objectID;

layout(binding =3) uniform sampler2D previousDepthTexture;
uniform float depthPeelingEpsilon;
uniform bool isPeeling;
uniform bool isNearToFar;
uniform int CureLayerNum;
uniform vec2 screenSize;

void main()
{	
	if(isPeeling && CureLayerNum>0){
		float currentDepth = gl_FragCoord.z;
		vec2 texCoord = gl_FragCoord.xy / screenSize;
		float previousDepth = texture(previousDepthTexture,texCoord).r;
		if(isNearToFar){
				//第一种情况，从近到远，逐层剥离
				//理论情况 ： currentDepth <= previousDepth 说明当前更近 剔除
				if(currentDepth <= (depthPeelingEpsilon+previousDepth))
					discard;
			}else{
	
				//第二种情况 从远到近，逐层剥离
				//理论情况 ： currentDepth > previousDepth 说明当前更远 core		
				if(currentDepth > (previousDepth-depthPeelingEpsilon))
					discard;
			}
		}
	FragColor = uvec4(objectID,gl_PrimitiveID,0,0);
}